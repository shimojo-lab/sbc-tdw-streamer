/*********************************
 *      frame_receiver.cpp      *
 *   (フレーム受信モジュール)   *
 ********************************/

#include "frame_receiver.hpp"

/* コンストラクタ */
FrameReceiver::FrameReceiver(_asio::io_service &ios, FrameViewer &viewer,int port):
    ios(ios),
    sock(ios),
    acc(ios, _ip::tcp::endpoint(_ip::tcp::v4(), port)),
    viewer(viewer)
{
    std::cout << "[Info] Waiting for TCP connection from server..." << std::endl;
    auto bind = boost::bind(&FrameReceiver::onConnect, this, _ph::error);
    this->acc.async_accept(this->sock, bind);
}

/* デストラクタ */
FrameReceiver::~FrameReceiver(){}

/* 接続時のコールバック関数 */
void FrameReceiver::onConnect(const _system::error_code &error){
    // 接続を通知
    if(error){
        std::cout << "[Error] Connection failed. (" << error.message() << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "[Info] Established TCP connection with server." << std::endl;
    
    // 受信処理をループ
    auto bind = boost::bind(&FrameReceiver::onReceive, this, _ph::error, _ph::bytes_transferred);
    while(true){
        _asio::async_read_until(this->sock, this->receive_buf, SEPARATOR, bind);
    }
    return;
}

/* 受信時のコールバック関数 */
inline void FrameReceiver::onReceive(const _system::error_code &error, size_t size){
    if(error){
        std::cerr << "[Error] Receive failed. (" << error.message() << ")" << std::endl;
    }else{
        const unsigned char *frame = _asio::buffer_cast<const unsigned char*>(this->receive_buf.data());
        std::cout << *frame << std::endl;
        //this->recieve_buf.consume(len);
        //this->viewer.displayFrame(frame);
    }
    return;
}

