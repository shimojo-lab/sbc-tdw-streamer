/******************************
*     frame_receiver.cpp      *
*      (フレーム受信器)       *
******************************/

#include "frame_receiver.hpp"

/* コンストラクタ */
FrameReceiver::FrameReceiver(_asio::io_service& ios, const msgbuf_ptr_t rbuf, const std::string ip, const int port):
    ios(ios),
    rbuf(rbuf),
    sock(ios)
{
    print_info("Ready for receiving TCP stream from " + ip + ":" + std::to_string(port));
    this->run(ip, port);
}

/* 受信処理を開始 */
void FrameReceiver::run(const std::string ip, const int port){
    const tcp_t::endpoint endpoint(_ip::address::from_string(ip), port);
    const auto bind = boost::bind(&FrameReceiver::onConnect, this, _ph::error);
    this->sock.async_connect(endpoint, bind);
    ios.run(); 
}

/* 接続時のコールバック */
void FrameReceiver::onConnect(const err_t& err){
    if(err){
        print_err("Failed TCP reconnection with head node", err.message());
        return;
    }
    print_info("Established TCP reconnection with head node");
    
    // フレーム受信を開始
    const auto bind = boost::bind(&FrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->stream_buf, MSG_DELIMITER, bind);
}

/* フレーム受信時のコールバック */
void FrameReceiver::onRecvFrame(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive frame", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // フレームを取得
    const auto data = this->stream_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<MSG_DELIMITER_LEN; ++i){
        recv_msg.pop_back();
    }
    this->rbuf->push(recv_msg);
    this->stream_buf.consume(t_bytes);
    
    // フレーム受信を再開
    const auto bind = boost::bind(&FrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->stream_buf, MSG_DELIMITER, bind); 
}

