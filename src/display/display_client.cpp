/*******************************
*      display_client.cpp      *
*  (ディスプレイクライアント)  *
*******************************/

#include "display_client.hpp"

/* コンストラクタ */
DisplayClient::DisplayClient(_asio::io_service& ios, ConfigParser& parser):
    ios(ios),
    sock(ios)
{
    // パラメータを設定
    std::tie(this->ip_addr, this->fs_port, this->fb_dev, this->tty_dev) = parser.getDisplayClientParams();
}

/* 初期化メッセージをパース */
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

/* ディスプレイクライアントを起動 */
void DisplayClient::run(){
    // ヘッドノードに接続
    err_t err;
    this->sock.connect(_ip::tcp::endpoint(_ip::address::from_string(this->ip_addr), this->fs_port), err);
    if(err){
        _ml::caution("Could not connect to head node", err.message());
        std::exit(EXIT_FAILURE);
    }
    _ml::notice("Connected to head node");
    
    // 初期化メッセージの受信を開始
    _asio::read_until(this->sock, this->stream_buf, MSG_DELIMITER, err);
    if(err){
        _ml::caution("Could not receive init message", err.message());
        std::exit(EXIT_FAILURE);
    }
    _ml::notice("Received init message from head node");
    
    // 初期化メッセージをパース
    const auto data = this->stream_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_end(data));
    recv_msg.erase(recv_msg.length()-MSG_DELIMITER_LEN);
    int width, height, stream_port, recvbuf_num, dec_thre_num, target_fps, tuning_term, yuv_format, quality;
    std::tie(
        width, height, stream_port, recvbuf_num, dec_thre_num, target_fps, tuning_term, yuv_format, quality
    ) = this->parseInitMsg(recv_msg);
    
    // 別スレッドでフレーム受信器を起動
    const tranbuf_ptr_t recv_buf = std::make_shared<TransceiveFramebuffer>(recvbuf_num);
    this->recv_thre = std::thread(std::bind(&DisplayClient::runFrameReceiver,
                                            this,
                                            stream_port,
                                            recv_buf)
    );
    
    // 別スレッドでフレーム展開器を起動
    const viewbuf_ptr_t view_buf = std::make_shared<ViewFramebuffer>(width, height, dec_thre_num+VIEWBUF_EXTRA_NUM);
    for(int i=0; i<dec_thre_num; ++i){
        this->dec_thres.push_back(
            std::thread(std::bind(&DisplayClient::runFrameDecoder,
                                  this,
                                  recv_buf,
                                  view_buf))
        );
    }
    
    // 同スレッドでフレーム表示器を起動
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

/* 別スレッドでフレーム受信器を起動 */
void DisplayClient::runFrameReceiver(const int stream_port, const tranbuf_ptr_t recv_buf){
    _asio::io_service ios;
    FrameReceiver receiver(ios, recv_buf);
    receiver.run(this->ip_addr, stream_port);
}

/* 別スレッドでフレーム展開器を起動 */
void DisplayClient::runFrameDecoder(const tranbuf_ptr_t recv_buf, const viewbuf_ptr_t view_buf){
    FrameDecoder decoder(recv_buf, view_buf);
    decoder.run();
}

