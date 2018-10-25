/*******************************
*      frame_receiver.cpp      *
*   (フレーム受信モジュール)   *
********************************/

#include "frame_receiver.hpp"

/* 定数の定義 */
const char* const SEPARATOR = "--boundary\r\n";  // TCPメッセージのセパレータ
const int SEPARATOR_NUM = std::strlen(SEPARATOR);    // TCPメッセージのセパレータの長さ

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
void FrameReceiver::onConnect(const _sys::error_code &error){
    // 接続を通知
    if(error){
        std::cout << "[Error] Connection failed. (" << error.message() << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "[Info] Established TCP connection with server." << std::endl;
    
    // 受信処理をループ
    auto bind = boost::bind(&FrameReceiver::onReceive, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->receive_buf, SEPARATOR, bind);
    return;
}

/* 受信時のコールバック関数 */
void FrameReceiver::onReceive(const _sys::error_code &error, std::size_t bytes_transferred){
    if(error){
        // 受信失敗を通知
        std::cerr << "[Error] Receive failed. (" << error.message() << ")" << std::endl;
    }else{
        // 受信用バッファからフレームのデータを抽出
        auto data = this->receive_buf.data();
        std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+bytes_transferred);
        for(int i=0; i<SEPARATOR_NUM; ++i){
            bytes_buf.pop_back();
        }
        std::vector<unsigned char> comp_buf(bytes_buf.c_str(), bytes_buf.c_str()+bytes_buf.length());
        this->viewer.displayFrame(comp_buf);
        
        // 受信処理を再開
        this->receive_buf.consume(bytes_transferred);
        auto bind = boost::bind(&FrameReceiver::onReceive, this, _ph::error, _ph::bytes_transferred);
        _asio::async_read_until(this->sock, this->receive_buf, SEPARATOR, bind);
    }
    return;
}

