/****************************
*   udp_frame_sender.cpp    *
*   (UDP版フレーム送信器)   *
****************************/

#include "udp_frame_sender.hpp"

/* コンストラクタ */
UDPFrameSender::UDPFrameSender(const ios_ptr_t ios, const fq_ptr_t queue, const int port, const bar_ptr_t barrier):
    BaseFrameSender(ios, queue, port, barrier),
{
    // 接続待機を開始
    print_info("Launched streaming server at :" + std::to_string(port));
    const auto bind = boost::bind(&UDPFrameSender::startConnect, this);
    this->strand.post(bind);
}

/* 接続待機を開始 */
void UDPFrameSender::startConnect(){
    const auto bind = boost::bind(&UDPFrameSender::onConnect, this, _ph::error);
    this->acc.async_accept(this->sock, this->starnd.wrap(bind));
}

/* TCP接続時のコールバック */
void UDPFrameSender::onConnect(const _err& err){
    // 接続元を表示
    const std::string ip = this->sock.remote_endpoint().address().to_string();
    if(err){
        print_err("Failed to establish TCP connection with " + ip, err.message());
        return;
    }
    print_info("Established TCP connection with " + ip);
    
    // フレーム送信を開始
    const cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&UDPFrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(this->compressFrame(frame)), this->strand.wrap(bind));
    
    // 同期メッセージ受信を開始
    const auto bind = boost::bind(&UDPFrameSender::onRecvSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, SEPARATOR, this->strand.wrap(bind));
}

/* TCPでのフレーム送信時のコールバック */
void UDPFrameSender::onSendFrame(const _err& err, size_t t_bytes){
    if(err){
        print_err("Failed to send frame", err.message());
    }
    
    // 次番のフレームを送信
    const cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&UDPFrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(this->compressFrame(frame)), this->strand.wrap(bind));
}

