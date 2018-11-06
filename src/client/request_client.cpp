/*****************************
*     request_client.cpp     *
*   (接続要求クライアント)   *
*****************************/

#include "request_client.hpp"

/* コンストラクタ */
RequestClient::RequestClient(_ios& ios, ConfigParser& parser):
    ios(ios),
    sock(ios)
{
    // フレーム表示用パラメータを設定
    std::tie(this->res_x, this->res_y, this->width, this->height) = parser.getFrameViewerParams();
    
    // ヘッドノードにTCP接続
    int port;
    std::tie(this->ip, port) = parser.getRequestClientParams();
    print_info("Connecting to " + std::string(this->ip) + ":" + std::to_string(port));
    const _tcp::endpoint endpoint(_ip::address::from_string(this->ip), port);
    const auto bind = boost::bind(&RequestClient::onConnect, this, _ph::error);
    this->sock.async_connect(endpoint, bind);
}

/* TCP接続時のコールバック関数 */
void RequestClient::onConnect(const _err& err){
    if(err){
        print_err("Failed to establish TCP connection with head node", err.message());
        return;
    }
    print_info("Established TCP connection with head node");
    
    // 初期化メッセージの受信を開始
    const auto bind = boost::bind(&RequestClient::onRecvInit, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, SEPARATOR, bind);
}

/* 初期化メッセージ受信時のコールバック */
void RequestClient::onRecvInit(const _err& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive init message", err.message());
        return;
    }
    print_info("Received init message from head node");
    
    // 初期化メッセージを取得
    const auto data = this->recv_buf.data();
    std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<SEPARATOR_LEN; ++i){
        bytes_buf.pop_back();
    }
    
    // 初期化メッセージをパース
    std::stringstream ss;
    ss << bytes_buf;
    _pt::ptree init_params;
    _pt::read_json(ss, init_params);
    const std::string protocol = init_params.get_optional<std::string>("protocol").get();
    const int port = std::stoi(init_params.get_optional<std::string>("port").get());
    const int framerate = std::stoi(init_params.get_optional<std::string>("framerate").get());
    const int frame_num = std::stoi(init_params.get_optional<std::string>("frame_num").get());
    
    // 別スレッドでフレーム受信器を起動
    _ios::strand strand(this->new_ios);
    const tcps_ptr_t new_sock = std::make_shared<_tcp::socket>(new_ios);
    const fq_ptr_t queue = std::make_shared<FrameQueue>(1);
    boost::thread(&RequestClient::runFrameReceiver, this, strand, new_sock, queue, port, protocol);
    
    // フレーム表示を開始
    FrameViewer viewer(this->new_ios, strand, new_sock, queue, this->res_x, this->res_y, this->width, this->height, framerate, frame_num);
    this->new_ios.run();
}

/* 別スレッドでフレーム受信器を起動 */
void RequestClient::runFrameReceiver(_ios::strand& strand, const tcps_ptr_t sock, const fq_ptr_t queue, const int port, const std::string protocol){
    if(protocol == "TCP"){
        TCPFrameReceiver receiver(this->new_ios, strand, sock, queue, this->ip, port);
    }else if(protocol == "UDP"){
        //UDPFrameReceiver receiver();
    }else{
        print_err("Invalid protocol is selected", protocol);
        return;
    }
    this->new_ios.run();
}

