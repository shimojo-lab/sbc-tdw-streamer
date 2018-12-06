/****************************
*   udp_frame_sender.cpp    *
*   (UDP版フレーム送信器)   *
****************************/

#include "udp_frame_sender.hpp"

/* コンストラクタ */
UDPFrameSender::UDPFrameSender(ios_t& ios, const fq_ptr_t queue, const int port):
    BaseFrameSender(ios, queue),
    sock(ios),
{
    // 接続待機を開始
    print_info("Launched streaming server at :" + std::to_string(port));
    this->startConnect();
}

/* 接続待機を開始 */
void TCPFrameSender::startConnect(){
    const auto bind = boost::bind(&TCPFrameSender::onConnect, this, _ph::error);
    this->acc.async_accept(this->sock, bind);
    this->ios.run();
}

/* TCP接続時のコールバック */
void TCPFrameSender::onConnect(const err_t& err){
    // 接続元を表示
    const std::string ip = this->sock.remote_endpoint().address().to_string();
    if(err){
        print_err("Failed to establish TCP connection with " + ip, err.message());
        return;
    }
    print_info("Established TCP connection with " + ip);
    
    // フレーム送信を開始
    const cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&TCPFrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(this->compressFrame(frame)), bind);
}

/* フレーム送信時のコールバック */
void TCPFrameSender::onSendFrame(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to send frame", err.message());
    }
    
    // 次番のフレームを送信
    const cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&TCPFrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(this->compressFrame(frame)), bind);
}

