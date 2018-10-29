/********************************
 *       frame_sender.cpp       *
 *     (分割フレーム送信器)     *
 ********************************/

#include "frame_sender.hpp"

const char* const SEPARATOR = "--boundary\r\n";  // 受信メッセージのセパレータ

/* コンストラクタ */
FrameSender::FrameSender(const smt_ios_t ios, const smt_fq_t queue, const char* const ip, const int port, const int protocol):
    ios(ios),
    tcp_sock(*ios),
    udp_sock(*ios, _ip::udp::endpoint(_ip::udp::v4(), port)),
    queue(queue),
    ip(ip),
    port(port),
    protocol(protocol)
{}

/* 送信処理を開始 */
void FrameSender::start(){
    // TCPで接続
    c_lock(), std::cout << "[Info] Connecting to '" << ip << ":" << port << "'..." << std::endl;
    const _ip::tcp::endpoint endpoint(_ip::address::from_string(this->ip), this->port);
    const auto bind = boost::bind(&FrameSender::onConnect, this, _ph::error);
    this->tcp_sock.async_connect(endpoint, bind);
    return;
}

/* TCP接続時のコールバック */
void FrameSender::onConnect(const _sys::error_code &error){
    if(error){
        c_lock(), std::cerr << "[Error] TCP Connection with '" << this->ip << ":" << this->port << "' failed. (" << error.message() << ")" << std::endl;
        return;
    }
    c_lock(), std::cout << "[Info] Established TCP Connection with '" << this->ip << ":" << this->port << "'." << std::endl;
    
    // 分割フレームの送信を開始
    cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&FrameSender::onInitialSend, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->tcp_sock, _asio::buffer(this->compressFrame(frame)), bind);
    return;
}

/* 初回メッセージ送信時のコールバック */
void FrameSender::onInitialSend(const _sys::error_code &error, std::size_t t_bytes){
    if(error){
        c_lock(), std::cout << "[Error] Initial messeage send failed. (" << error.message() << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    // 次番のフレームを送信
    cv::Mat frame = this->queue->dequeue();
    if(this->protocol == 0){  // TCPで送信
        const auto bind = boost::bind(&FrameSender::onTCPSend, this, _ph::error, _ph::bytes_transferred);
        _asio::async_write(this->tcp_sock, _asio::buffer(this->compressFrame(frame)), bind);
    }else if(this->protocol == 1){  // UDPで送信
        
    }
    return;
}

/* TCPでの分割フレーム送信時のコールバック */
void FrameSender::onTCPSend(const _sys::error_code &error, std::size_t t_bytes){
    if(error){
        c_lock(), std::cout << "[Error] TCP send failed. (" << error.message() << ")" << std::endl;
    }
    
    // 次番のフレームを送信
    cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&FrameSender::onTCPSend, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->tcp_sock, _asio::buffer(this->compressFrame(frame)), bind);
    return;
}

/* UDPでの分割フレーム送信時のコールバック */
void FrameSender::onUDPSend(const _sys::error_code &error, std::size_t t_bytes){
    if(error){
        c_lock(), std::cout << "[Error] UDP send failed. (" << error.message() << ")" << std::endl;
    }
    
    // 次番のフレームを送信
    cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&FrameSender::onUDPSend, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->tcp_sock, _asio::buffer(this->compressFrame(frame)), bind);
    return;
}

