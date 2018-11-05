/*****************************
*     request_client.cpp     *
*   (接続要求クライアント)   *
*****************************/

#include "request_client.hpp"

/* コンストラクタ */
RequestClient::RequestClient(const ios_ptr_t ios, ConfigParser& parser):
    ios(ios),
    sock(*ios)
{
    // フレーム表示用パラメータを設定
    std::tie(this->res_x, this->res_y, this->width, this->height) = parser.getFrameViewerParams();
    
    // ヘッドノードにTCP接続
    int port;
    std::tie(this->ip, port) = parser.getRequestClientParams();
    print_info("Connecting to " + std::string(this->ip) + ":" + std::to_string(port));
    const _ip::tcp::endpoint endpoint(_ip::address::from_string(this->ip), port);
    const auto bind = boost::bind(&RequestClient::onConnect, this, _ph::error);
    this->sock.async_connect(endpoint, bind);
}

/* TCP接続時のコールバック関数 */
void RequestClient::onConnect(const _sys::error_code& err){
    if(err){
        print_err("Failed to establish TCP connection with head node", err.message());
        exit(EXIT_FAILURE);
    }
    print_info("Established TCP connection with head node.");
    
    // 初期化メッセージの受信を開始
    const auto bind = boost::bind(&RequestClient::onRecvInit, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, SEPARATOR, bind);
    return;
}

/* 初期化メッセージ受信時のコールバック */
void RequestClient::onRecvInit(const _sys::error_code& err, std::size_t t_bytes){
    if(err){
        print_err("Failed to receive init message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 初期化メッセージを取得
    const auto data = this->recv_buf.data();
    std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<SEPARATOR_LEN; ++i){
        bytes_buf.pop_back();
    }
    
    // 初期化メッセージをパース
    std::stringstream ss;
    _pt::ptree init_params;
    _pt::read_json(ss, init_params);
    const int id = std::stoi(init_params.get_optional<std::string>("id").get());
    const std::string protocol = init_params.get_optional<std::string>("protocol").get();
    const int port = std::stoi(init_params.get_optional<std::string>("port").get());
    const int framerate = std::stoi(init_params.get_optional<std::string>("framerate").get());
    const int frame_num = std::stoi(init_params.get_optional<std::string>("frame_num").get());
    
print_info(std::to_string(id));
    // 別スレッドでフレーム受信器を起動
    const tcps_ptr_t tcp_sock = std::make_shared<_ip::tcp::socket>(*(this->ios));
    const fq_ptr_t queue = std::make_shared<FrameQueue>(1);
    boost::thread(&RequestClient::runFrameReceiver, this, tcp_sock, queue, port, protocol);
    
    // フレーム表示を開始
    boost::barrier barrier(2);
    FrameViewer viewer(tcp_sock, queue, this->res_x, this->res_y, this->width, this->height, framerate, frame_num, barrier);
    viewer.run();
    return;
}

/* 別スレッドでフレーム受信器を起動 */
void RequestClient::runFrameReceiver(const tcps_ptr_t tcp_sock, const fq_ptr_t queue, const int port, const std::string protocol){
    FrameReceiver receiver(this->ios, tcp_sock, queue, this->ip, port, protocol);
    this->ios->run();
    return;
}

