/******************************
*   udp_frame_receiver.cpp    *
*    (UDP版フレーム受信器)    *
******************************/

#include "udp_frame_receiver.hpp"

/* コンストラクタ */
UDPFrameReceiver::UDPFrameReceiver(ios_t& ios, const fq_ptr_t queue, const std::string ip, const int port):
    BaseFrameReceiver(ios, queue),
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
        print_err("Failed to connect to head node", err.message());
        return;
    }
    print_info("Established TCP connection with head node");
    
    // フレーム受信を開始
    const auto bind = boost::bind(&TCPFrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, SEPARATOR, bind);
}

/* TCPでのフレーム受信時のコールバック */
void TCPFrameReceiver::onRecvFrame(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive frame", err.message());
        return;
    }
    
    // フレームを取得
    const auto data = this->recv_buf.data();
    std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<SEPARATOR_LEN; ++i){
        bytes_buf.pop_back();
    }
    std::vector<unsigned char> comp_buf(bytes_buf.c_str(), bytes_buf.c_str()+bytes_buf.length());
    this->queue->enqueue(comp_buf);
    this->recv_buf.consume(t_bytes);
    
    // フレーム受信を再開
    const auto bind = boost::bind(&TCPFrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, SEPARATOR, bind); 
}

