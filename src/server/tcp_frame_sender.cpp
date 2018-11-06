/****************************
*   tcp_frame_sender.cpp    *
*   (TCP版フレーム送信器)   *
****************************/

#include "tcp_frame_sender.hpp"

/* コンストラクタ */
TCPFrameSender::TCPFrameSender(_ios& ios, const fq_ptr_t queue, const int port, const bar_ptr_t barrier):
    BaseFrameSender(ios, queue, port, barrier)
{
    // 接続待機を開始
    print_info("Launched streaming server at :" + std::to_string(port));
    const auto bind = boost::bind(&TCPFrameSender::startConnect, this);
    this->strand.post(bind);
    ios.run();
}

/* 接続待機を開始 */
void TCPFrameSender::startConnect(){
print_debug("aaaa");
    const auto bind = boost::bind(&TCPFrameSender::onConnect, this, _ph::error);
    this->acc.async_accept(this->sock, this->strand.wrap(bind));
}

/* TCP接続時のコールバック */
void TCPFrameSender::onConnect(const _err& err){
    // 接続元を表示
    const std::string ip = this->sock.remote_endpoint().address().to_string();
    if(err){
        print_err("Failed to establish TCP connection with " + ip, err.message());
        return;
    }
    print_info("Established TCP connection with " + ip);
    
    // フレーム送信を開始
    const cv::Mat frame = this->queue->dequeue();
    const auto send_bind = boost::bind(&TCPFrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(this->compressFrame(frame)), this->strand.wrap(send_bind));
    
    // 同期メッセージ受信を開始
    const auto sync_bind = boost::bind(&TCPFrameSender::onRecvSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, SEPARATOR, this->strand.wrap(sync_bind));
}

/* TCPでのフレーム送信時のコールバック */
void TCPFrameSender::onSendFrame(const _err& err, size_t t_bytes){
    if(err){
        print_err("Failed to send frame", err.message());
    }
    
    // 次番のフレームを送信
    const cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&TCPFrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(this->compressFrame(frame)), this->strand.wrap(bind));
}

