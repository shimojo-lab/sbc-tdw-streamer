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
    int port, rbuf_size, vbuf_size;
    std::tie(this->ip, port, this->fb_dev, rbuf_size, vbuf_size) = parser.getDisplayClientParams();
    this->rbuf = std::make_shared<RingBuffer<std::string>>(DYNAMIC_BUF, rbuf_size);
    this->vbuf = std::make_shared<RingBuffer<cv::Mat>>(DYNAMIC_BUF, vbuf_size);
    
    // ヘッドノードに接続
    this->sock.async_connect(_ip::tcp::endpoint(_ip::address::from_string(this->ip), port),
                             boost::bind(&DisplayClient::onConnect, this, _ph::error)
    );
}

/* ヘッドノード接続時のコールバック */
void DisplayClient::onConnect(const err_t& err){
    if(err){
        print_err("Failed connection with head node", err.message());
        std::exit(EXIT_FAILURE);
    }
    print_info("Connected to connect to head node");
    
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
        print_err("Failed to receive init message", err.message());
        return;
    }
    print_info("Received init message from head node");
    
    // 初期化メッセージをパース
    const auto data = this->stream_buf.data();
    std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<MSG_DELIMITER_LEN; ++i){
        bytes_buf.pop_back();
    }
    std::stringstream ss;
    _pt::ptree init_params;
    int id, row, column, width, height, port, protocol_type;
    ss << bytes_buf;
    _pt::read_json(ss, init_params);
    id = std::stoi(init_params.get_optional<std::string>("id").get());
    port = std::stoi(init_params.get_optional<std::string>("port").get());
    
    // 別スレッドでフレーム受信器を起動
    this->recv_thre = boost::thread(boost::bind(&DisplayClient::runFrameReceiver, this, port));
    
    // 別スレッドでフレーム展開器を起動
    for(int i=0; i<1; ++i){
        this->dec_thres.push_back(boost::thread(boost::bind(&DisplayClient::runFrameDecoder, this)));
    }
    
    // フレーム表示器を起動
    print_info("Start playback video");
    FrameViewer viewer(this->ios, this->sock, this->vbuf, 0.7, this->fb_dev);
}

/* 別スレッドでフレーム展開器を起動 */
void DisplayClient::runFrameDecoder(){
    FrameDecoder decoder(this->rbuf, this->vbuf, roi, this->res_x, this->res_y);
    decoder.run();
}

/* 別スレッドでフレーム受信器を起動 */
void DisplayClient::runFrameReceiver(const int port){
    _asio::io_service ios;
    FrameReceiver receiver(ios, this->rbuf, this->ip, port);
}

