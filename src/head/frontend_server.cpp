/************************************
*        frontend_server.cpp        *
*  (class for the frontend server)  *
************************************/

#include "frontend_server.hpp"

/* constructor */
FrontendServer::FrontendServer(_asio::io_service& ios, ConfigParser& parser, const int fs_port):
    ios(ios),
    acc(ios, _ip::tcp::endpoint(_ip::tcp::v4(), fs_port))
{
    // get the parameters from the config parser
    std::string src, ycbcr_format_name;
    int column, row, bezel_w, bezel_h, width, height, stream_port, sendbuf_num, recvbuf_num;
    int target_fps, quality, dec_thre_num, tuning_term;
    double fps_jitter;
    std::tie(
        src, target_fps, fps_jitter, column, row, bezel_w, bezel_h, width, height, stream_port,
        sendbuf_num, recvbuf_num, ycbcr_format_name, quality, dec_thre_num, tuning_term, this->ip_addrs
    ) = parser.getFrontendServerParams();
    this->display_num = column * row;
    
    // set the initial YCbCr format
    int ycbcr_format;
    if(ycbcr_format_name == "4:4:4"){
        ycbcr_format = TJSAMP_444;
    }else if(ycbcr_format_name == "4:2:2"){
        ycbcr_format = TJSAMP_422;
    }else if(ycbcr_format_name == "4:2:0"){ 
        ycbcr_format = TJSAMP_420;
    }else{
        _ml::caution("YCbCr format is invalid", "Check config file");
        std::exit(EXIT_FAILURE);
    }
    
    // set the parameters packed in the initial message
    this->init_params.setIntParam("width", width);
    this->init_params.setIntParam("height", height);
    this->init_params.setIntParam("stream_port", stream_port);
    this->init_params.setIntParam("target_fps", target_fps);
    this->init_params.setDoubleParam("fps_jitter", fps_jitter);
    this->init_params.setIntParam("recvbuf_num", recvbuf_num);
    this->init_params.setIntParam("dec_thre_num", dec_thre_num);
    this->init_params.setIntParam("tuning_term", tuning_term);
    this->init_params.setIntParam("ycbcr_format", ycbcr_format);
    this->init_params.setIntParam("quality", quality);
    
    // set the other parameters
    this->sock = std::make_shared<_ip::tcp::socket>(ios);
    this->socks = std::vector<sock_ptr_t>(this->display_num);
    this->send_bufs = std::vector<tranbuf_ptr_t>(this->display_num);
    this->ycbcr_format_list = jpeg_params_t(this->display_num);
    this->quality_list = jpeg_params_t(this->display_num);
    for(int i=0; i<this->display_num; ++i){
        this->send_bufs[i] = std::make_shared<TransceiveFramebuffer>(sendbuf_num);
        this->ycbcr_format_list[i].store(ycbcr_format, std::memory_order_release);
        this->quality_list[i].store(quality, std::memory_order_release);
    }
    
    // launch the encoder thread
    this->enc_thre = std::thread(std::bind(&FrontendServer::runFrameEncoder,
                                           this,
                                           src,
                                           column,
                                           row,
                                           bezel_w,
                                           bezel_h,
                                           width,
                                           height)
    );
    
    // launch the sender thread
    this->send_thre = std::thread(std::bind(&FrontendServer::runFrameSender,
                                            this,
                                            stream_port,
                                            dec_thre_num+VIEWBUF_EXTRA_NUM)
    );
    
    // start waiting for the display node connection
    _ml::notice("Waiting for display node connection at :" + std::to_string(fs_port));
    this->waitForConnection();
}

/* start waiting for the display node connection */
void FrontendServer::waitForConnection(){
    this->acc.async_accept(*this->sock,
                           boost::bind(&FrontendServer::onConnect, this, _ph::error)
    );
}

/* the callback when connected by the display node */
void FrontendServer::onConnect(const err_t& err){
    const std::string ip_addr = this->sock->remote_endpoint().address().to_string();
    if(err){
        _ml::caution("Could not accept " + ip_addr, err.message());
        std::exit(EXIT_FAILURE);
        return;
    }
    
    // check the ID of the display node
    const auto iter = std::find(this->ip_addrs.begin(), this->ip_addrs.end(), ip_addr);
    const size_t id = std::distance(this->ip_addrs.begin(), iter);
    if(id == this->ip_addrs.size()){
        _ml::caution(ip_addr + " is not registered", "Check config file");
        std::exit(EXIT_FAILURE);
        return;
    }else{
        _ml::notice("Accepted new display node: " + ip_addr);
    }
    
    // send the initial message to the display node
    const std::string send_msg = this->init_params.serialize() + MSG_DELIMITER;
    _asio::async_write(*this->sock,
                       _asio::buffer(send_msg),
                       boost::bind(&FrontendServer::onSendInit, this, _ph::error, _ph::bytes_transferred, ip_addr)
    );
    
    // prepare for a new TCP socket
    this->socks[id] = this->sock;
    this->sock = std::make_shared<_ip::tcp::socket>(this->ios);
}

/* the callback when sending the initial message */
void FrontendServer::onSendInit(const err_t& err, size_t t_bytes, const std::string ip){
    if(err){
        _ml::caution("Failed to send init message to " + ip, err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // repeat the same process until all the display nodes connect
    ++this->connected_num;
    if(this->connected_num < this->display_num){
        this->waitForConnection();
    }else{
        // launch the sync manager in this thread
        _ml::notice("All display nodes connected");
        this->sock->close();
        this->runSyncManager();
    }
}

/* launch the frame encoder */
void FrontendServer::runFrameEncoder(const std::string video_src, const int column, const int row,
                                     const int bezel_w, const int bezel_h, const int width, const int height)
{
    FrameEncoder encoder(video_src,
                         column,
                         row,
                         bezel_w,
                         bezel_h,
                         width,
                         height,
                         this->ycbcr_format_list,
                         this->quality_list,
                         this->send_bufs
    );
    encoder.run();
}

/* launch the frame sender */
void FrontendServer::runFrameSender(const int stream_port, const int viewbuf_num){
    _asio::io_service ios;
    FrameSender sender(ios,
                       stream_port,
                       this->display_num,
                       this->send_bufs,
                       viewbuf_num
    );
}

/* launch the sync manager */
void FrontendServer::runSyncManager(){
    SyncManager manager(this->ios,
                        this->socks,
                        this->ycbcr_format_list,
                        this->quality_list
    );
    manager.run();
}

