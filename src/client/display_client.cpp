/*******************************
*      display_client.cpp      *
*  (ディスプレイクライアント)  *
*******************************/

#include "display_client.hpp"

/* コンストラクタ */
DisplayClient::DisplayClient(ios_t& ios, ConfigParser& parser):
    ios(ios),
    sock(ios)
{
    // パラメータを設定
    int port, rbuf_size, vbuf_size;
    std::tie(this->ip, port, this->res_x, this->res_y, rbuf_size, vbuf_size) = parser.getDisplayClientParams();
    this->rbuf = std::make_shared<RingBuffer<std::string>>(DYNAMIC_BUF, rbuf_size);
    this->vbuf = std::make_shared<RingBuffer<cv::Mat>>(DYNAMIC_BUF, vbuf_size);
    
    // ヘッドノードに接続
    print_info("Connecting to head node (" + std::string(this->ip) + ":" + std::to_string(port) + ")");
    const tcp_t::endpoint endpoint(_ip::address::from_string(this->ip), port);
    const auto bind = boost::bind(&DisplayClient::onConnect, this, _ph::error);
    this->sock.async_connect(endpoint, bind);
}

/* ヘッドノード接続時のコールバック */
void DisplayClient::onConnect(const err_t& err){
    if(err){
        print_err("Failed TCP connection with head node", err.message());
        std::exit(EXIT_FAILURE);
    }
    print_info("Established TCP connection with head node");
    
    // 初期化メッセージの受信を開始
    const auto bind = boost::bind(&DisplayClient::onRecvInit, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, MSG_DELIMITER, bind);
}

/* 初期化メッセージ受信時のコールバック */
void DisplayClient::onRecvInit(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive init message", err.message());
        return;
    }
    print_info("Received init message from head node");
    
    // 初期化メッセージをパース
    const auto data = this->recv_buf.data();
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
    row = std::stoi(init_params.get_optional<std::string>("row").get());
    column = std::stoi(init_params.get_optional<std::string>("column").get());
    width = std::stoi(init_params.get_optional<std::string>("width").get());
    height = std::stoi(init_params.get_optional<std::string>("height").get());
    port = std::stoi(init_params.get_optional<std::string>("port").get());
    protocol_type = std::stoi(init_params.get_optional<std::string>("protocol_type").get());
    
    // 別スレッドでフレーム受信器を起動
    this->receiver_thre = boost::thread(boost::bind(&DisplayClient::runFrameReceiver, this, port, protocol_type));
    
    // 別スレッドでフレーム展開器を起動
    this->decoder_thre = boost::thread(boost::bind(&DisplayClient::runFrameDecoder, this, id, row, column, width, height));
    
    // フレーム表示器を起動
    print_info("Start playback video");
    FrameViewer viewer(this->ios, this->sock, this->vbuf, 0.7);
}

/* 別スレッドでフレーム展開器を起動 */
void DisplayClient::runFrameDecoder(const int id, const int row, const int column, const int width, const int height){
    const int point_y = int(id / row);
    const int point_x = id - point_y * row;
    const int roi_x = int(width / row);
    const int roi_y = int(height / column);
    const cv::Rect roi(point_x*roi_x, point_y*roi_y, roi_x, roi_y);
    FrameDecoder decoder(this->rbuf, this->vbuf, roi, this->res_x, this->res_y);
    decoder.run();
}

/* 別スレッドでフレーム受信器を起動 */
void DisplayClient::runFrameReceiver(const int port, const int protocol_type){
    ios_t ios;
    if(protocol_type == 0){
        TCPFrameReceiver receiver(ios, this->rbuf, this->ip, port);
    }else if(protocol_type == 1){
        UDPFrameReceiver receiver(ios, this->rbuf, port);
    }else{
        print_err("Invalid protocol is selected", "Select from TCP or UDP");
    }
}

