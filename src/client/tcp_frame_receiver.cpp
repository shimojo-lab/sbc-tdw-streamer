/******************************
*   tcp_frame_receiver.cpp    *
*    (TCP版フレーム受信器)    *
******************************/

#include "tcp_frame_receiver.hpp"

/* コンストラクタ */
TCPFrameReceiver::TCPFrameReceiver(ios_t& ios, const msgbuf_ptr_t rbuf, const std::string ip, const int port):
    BaseFrameReceiver(ios, rbuf),
    sock(ios)
{
    print_info("Reconnecting to " + ip + ":" + std::to_string(port));
    this->startConnect(ip, port);
}

/* TCP接続待機を開始 */
void TCPFrameReceiver::startConnect(const std::string ip, const int port){
    const tcp_t::endpoint endpoint(_ip::address::from_string(ip), port);
    const auto bind = boost::bind(&TCPFrameReceiver::onConnect, this, _ph::error);
    this->sock.async_connect(endpoint, bind);
    ios.run(); 
}

/* TCP接続時のコールバック */
void TCPFrameReceiver::onConnect(const err_t& err){
    if(err){
        print_err("Failed TCP connection with head node", err.message());
        return;
    }
    print_info("Established TCP connection with head node");
    
    // フレーム受信を開始
    const auto bind = boost::bind(&TCPFrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, MSG_DELIMITER, bind);
}

/* TCPでのフレーム受信時のコールバック */
void TCPFrameReceiver::onRecvFrame(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive frame", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // フレームを取得
    const auto data = this->recv_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<MSG_DELIMITER_LEN; ++i){
        recv_msg.pop_back();
    }
    this->rbuf->push(recv_msg);
    this->recv_buf.consume(t_bytes);
    
    // フレーム受信を再開
    const auto bind = boost::bind(&TCPFrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, MSG_DELIMITER, bind); 
}

