/*****************************
*     request_client.cpp     *
*   (接続要求クライアント)   *
*****************************/

#include "request_client.hpp"

/* コンストラクタ */
RequestClient::RequestClient(ios_t& ios, ConfigParser& parser):
    ios(ios),
    sock(ios)
{
    // フレーム表示用パラメータを設定
    std::tie(this->res_x, this->res_y, this->width, this->height) = parser.getFrameViewerParams();
    
    // ヘッドノードにTCP接続
    int port;
    std::tie(this->ip, port) = parser.getRequestClientParams();
    print_info("Connecting to " + std::string(this->ip) + ":" + std::to_string(port));
    const tcp_t::endpoint endpoint(_ip::address::from_string(this->ip), port);
    const auto bind = boost::bind(&RequestClient::onConnect, this, _ph::error);
    this->sock.async_connect(endpoint, bind);
}

/* TCP接続時のコールバック関数 */
void RequestClient::onConnect(const err_t& err){
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
void RequestClient::onRecvInit(const err_t& err, size_t t_bytes){
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
    _pt::ptree init_params;
    std::string protocol;
    int port, frame_num, frame_rate;
    
    ss << bytes_buf;
    _pt::read_json(ss, init_params);
    protocol = init_params.get_optional<std::string>("protocol").get();
    port = std::stoi(init_params.get_optional<std::string>("port").get());
    frame_num = std::stoi(init_params.get_optional<std::string>("frame_num").get());
    frame_rate = std::stoi(init_params.get_optional<std::string>("frame_rate").get());
    
    // 別スレッドでフレーム受信器を起動
    const fq_ptr_t queue = std::make_shared<FrameQueue>(1);
    const auto bind = boost::bind(&RequestClient::runFrameReceiver, this, queue, protocol, port);
    this->recv_thre = boost::thread(bind);
    
    // フレーム表示を開始
    FrameViewer viewer(this->ios, this->sock, queue, this->res_x, this->res_y, this->width, this->height, frame_num, frame_rate);
}

/* 別スレッドでフレーム受信器を起動 */
void RequestClient::runFrameReceiver(const fq_ptr_t queue, const std::string protocol, const int port){
    ios_t ios;
    if(protocol == "TCP"){
        TCPFrameReceiver receiver(ios, queue, this->ip, port);
    }else if(protocol == "UDP"){
        //UDPFrameReceiver receiver();
    }else{
        print_err("Invalid protocol is selected", protocol);
        return;
    }
}

