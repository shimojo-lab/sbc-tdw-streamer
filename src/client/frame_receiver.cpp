/*****************************
*     frame_receiver.cpp     *
*    (分割フレーム受信器)    *
******************************/

#include "frame_receiver.hpp"

/* 定数の定義 */
const char* const SEPARATOR = "--boundary\r\n";    // 受信メッセージのセパレータ
const int SEPARATOR_LEN = std::strlen(SEPARATOR);  // 受信メッセージのセパレータの長さ

/* コンストラクタ */
FrameReceiver::FrameReceiver(_asio::io_service &ios, smt_FrameQueue_t queue, int port):
    ios(ios),
    sock(ios),
    acc(ios, _ip::tcp::endpoint(_ip::tcp::v4(), port)),
    queue(queue)
{
    // TCP接続待機を開始
    std::cout << "[Info] Waiting for TCP connection..." << std::endl;
    auto bind = boost::bind(&FrameReceiver::onTCPConnect, this, _ph::error);
    this->acc.async_accept(this->sock, bind);
}

/* デストラクタ */
FrameReceiver::~FrameReceiver(){}

/* TCP接続時のコールバック関数 */
void FrameReceiver::onTCPConnect(const _sys::error_code &error){
    if(error){
        std::cout << "[Error] TCP Connection failed. (" << error.message() << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "[Info] Established TCP connection with head node." << std::endl;
    
    // 受信処理を開始
    auto bind = boost::bind(&FrameReceiver::onReceive, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->receive_buf, SEPARATOR, bind);
    return;
}

/* 受信時のコールバック */
void FrameReceiver::onReceive(const _sys::error_code &error, std::size_t bytes_transferred){
    if(error){
        std::cerr << "[Warning] Receive failed. (" << error.message() << ")" << std::endl;
    }else{
        // 受信用バッファからフレームのデータを抽出
        auto data = this->receive_buf.data();
        std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+bytes_transferred);
        for(int i=0; i<SEPARATOR_LEN; ++i){
            bytes_buf.pop_back();
        }
        std::vector<unsigned char> comp_buf(bytes_buf.c_str(), bytes_buf.c_str()+bytes_buf.length());
        this->queue->enqueue(comp_buf);
    }
        
        // 受信処理を再開
    this->receive_buf.consume(bytes_transferred);
    auto bind = boost::bind(&FrameReceiver::onReceive, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->receive_buf, SEPARATOR, bind);
    return;
}

