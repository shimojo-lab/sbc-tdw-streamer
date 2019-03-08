/***************************************
*          display_client.cpp          *
*  (the class for the display client)  *
***************************************/

#include "display_client.hpp"

/* constructor */
DisplayClient::DisplayClient(_asio::io_service& ios, ConfigParser& parser):
    ios(ios),
    sock(ios)
{
    // set the parameters
    int fs_port;
    std::tie(this->ip_addr, fs_port, this->fb_dev, this->tty_dev) = parser.getDisplayClientParams();
    
    // connect to the head node
    this->sock.async_connect(_ip::tcp::endpoint(_ip::address::from_string(this->ip_addr), fs_port),
                             boost::bind(&DisplayClient::onConnect, this, _ph::error)
    );
}

/* parse the initial message */
const init_params_t DisplayClient::parseInitMsg(const std::string& msg){
    JsonHandler init_params;
    init_params.deserialize(msg);
    const int width = init_params.getIntParam("width");
    const int height = init_params.getIntParam("height");
    const int stream_port = init_params.getIntParam("stream_port");
    const int target_fps = init_params.getIntParam("target_fps");
    const int recvbuf_num = init_params.getIntParam("recvbuf_num");
    const int dec_thre_num = init_params.getIntParam("dec_thre_num");
    const int tuning_term = init_params.getIntParam("tuning_term");
    const int sampling_type = init_params.getIntParam("yuv_format");
    const int quality = init_params.getIntParam("quality");
    return std::forward_as_tuple(
        width, height, stream_port, recvbuf_num, dec_thre_num, target_fps, tuning_term, sampling_type, quality
    );
}

/* the callback when connecting to the head node */
void DisplayClient::onConnect(const err_t& err){
    if(err){
        _ml::caution("Could not connect to head node", err.message());
        std::exit(EXIT_FAILURE);
    }
    _ml::notice("Connected to head node");
    
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&DisplayClient::onRecvInitMsg, this, _ph::error, _ph::bytes_transferred)
    );
}

/* the callback when receiving the initial message */
void DisplayClient::onRecvInitMsg(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Could not receive init message", err.message());
        return;
    }
    _ml::notice("Received init message from head node");
    
    // parse the initial message
    const auto data = this->stream_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_end(data));
    recv_msg.erase(recv_msg.length()-MSG_DELIMITER_LEN);
    int width, height, stream_port, recvbuf_num, dec_thre_num, target_fps, tuning_term, yuv_format, quality;
    std::tie(
        width, height, stream_port, recvbuf_num, dec_thre_num, target_fps, tuning_term, yuv_format, quality
    ) = this->parseInitMsg(recv_msg);
    
    // launch the receiver thread
    const tranbuf_ptr_t recv_buf = std::make_shared<TransceiveFramebuffer>(recvbuf_num);
    this->recv_thre = std::thread(std::bind(&DisplayClient::runFrameReceiver,
                                            this,
                                            stream_port,
                                            recv_buf)
    );
    
    // launch the decoder threads
    const viewbuf_ptr_t view_buf = std::make_shared<ViewFramebuffer>(width, height, dec_thre_num+VIEWBUF_EXTRA_NUM);
    for(int i=0; i<dec_thre_num; ++i){
        this->dec_thres.push_back(
            std::thread(std::bind(&DisplayClient::runFrameDecoder,
                                  this,
                                  recv_buf,
                                  view_buf))
        );
    }
    
    // launch the frame viewer in this thread
    SyncMessageGenerator generator(target_fps, tuning_term, recv_buf, yuv_format, quality);
    FrameViewer viewer(this->ios,
                       this->sock,
                       view_buf,
                       this->fb_dev,
                       width,
                       height,
                       this->tty_dev,
                       generator
    );
}

/* launch the frame receiver */
void DisplayClient::runFrameReceiver(const int stream_port, const tranbuf_ptr_t recv_buf){
    _asio::io_service ios;
    FrameReceiver receiver(ios, this->ip_addr, stream_port, recv_buf);
}

/* launch the frame decoder */
void DisplayClient::runFrameDecoder(const tranbuf_ptr_t recv_buf, const viewbuf_ptr_t view_buf){
    FrameDecoder decoder(recv_buf, view_buf);
    decoder.run();
}

