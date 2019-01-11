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
    int port;
    std::tie(this->ip, port, this->fb_dev, this->tty_dev) = parser.getDisplayClientParams();
    
    // ヘッドノードに接続
    this->sock.async_connect(_ip::tcp::endpoint(_ip::address::from_string(this->ip), port),
                             boost::bind(&DisplayClient::onConnect, this, _ph::error)
    );
}

/* ヘッドノード接続時のコールバック */
void DisplayClient::onConnect(const err_t& err){
    if(err){
        _ml::caution("Failed connection with head node", err.message());
        std::exit(EXIT_FAILURE);
    }
    _ml::notice("Connected to connect to head node");
    
    // 初期化メッセージの受信を開始
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&DisplayClient::onRecvInit, this, _ph::error, _ph::bytes_transferred)
    );
}

/* 初期化メッセージ受信時のコールバック */
void DisplayClient::onRecvInit(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Failed to receive init message", err.message());
        return;
    }
    _ml::notice("Received init message from head node");
    
    // 初期化メッセージをパース
    std::stringstream ss;
    _pt::ptree params;
    const auto data = this->stream_buf.data();
    std::string params_bytes(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    params_bytes.erase(params_bytes.length()-MSG_DELIMITER_LEN);
    ss << params_bytes;
    _pt::read_json(ss, params);
    const int width = std::stoi(params.get_optional<std::string>("width").get());
    const int height = std::stoi(params.get_optional<std::string>("height").get());
    const int stream_port = std::stoi(params.get_optional<std::string>("stream_port").get());
    const int recvbuf_num = std::stoi(params.get_optional<std::string>("recvbuf_num").get());
    const int wait_usec = std::stoi(params.get_optional<std::string>("wait_usec").get());
    const int dec_thre_num = std::stoi(params.get_optional<std::string>("dec_thre_num").get());
    
    // 別スレッドでフレーム受信器を起動
    const tranbuf_ptr_t recv_buf = std::make_shared<TransceiveFramebuffer>(recvbuf_num, wait_usec);
    this->recv_thre = boost::thread(boost::bind(&DisplayClient::runFrameReceiver,
                                                this,
                                                stream_port,
                                                recv_buf)
    );
    
    // 別スレッドでフレーム展開器を起動
    const rawbuf_ptr_t view_buf = std::make_shared<ViewFramebuffer>(width, height, dec_thre_num+1, wait_usec);
    for(int i=0; i<dec_thre_num; ++i){
        this->dec_thres.push_back(boost::thread(boost::bind(&DisplayClient::runFrameDecoder,
                                                            this,
                                                            recv_buf,
                                                            view_buf)
        ));
    }
    
    // 同スレッドでフレーム表示器を起動
    FrameViewer viewer(this->ios,
                       this->sock,
                       recv_buf,
                       view_buf,
                       this->fb_dev,
                       width,
                       height,
                       this->tty_dev
    );
}

/* 別スレッドでフレーム受信器を起動 */
void DisplayClient::runFrameReceiver(const int stream_port, const tranbuf_ptr_t recv_buf){
    _asio::io_service ios;
    FrameReceiver receiver(ios, this->ip, stream_port, recv_buf);
}

/* 別スレッドでフレーム展開器を起動 */
void DisplayClient::runFrameDecoder(const tranbuf_ptr_t recv_buf, const rawbuf_ptr_t view_buf){
    FrameDecoder decoder(recv_buf, view_buf);
    decoder.run();
}

