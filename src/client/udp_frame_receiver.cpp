/******************************
*   udp_frame_receiver.cpp    *
*    (UDP版フレーム受信器)    *
******************************/

#include "udp_frame_receiver.hpp"

/* コンストラクタ */
UDPFrameReceiver::UDPFrameReceiver(ios_t& ios, const msgbuf_ptr_t rbuf, const int port):
    BaseFrameReceiver(ios, rbuf),
    sock(ios, udp_t::endpoint(udp_t::v4(), port))
{
    print_info("Started to receive UDP at :" + std::to_string(port));
    this->run("", port);
}

/* 受信処理を開始 */
void UDPFrameReceiver::run(const std::string ip, const int port){
    const auto bind = boost::bind(&UDPFrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred);
    this->sock.async_receive_from(_asio::buffer(this->stream_buf), this->endpoint, bind);
    ios.run();
}

/* フレーム受信時のコールバック */
void UDPFrameReceiver::onRecvFrame(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive frame", err.message());
        return;
    }
    
    // フレームを取得
    const auto data = this->stream_buf.data();
    std::string recv_msg(data, data+t_bytes);
    for(int i=0; i<MSG_DELIMITER_LEN; ++i){
        recv_msg.pop_back();
    }
    this->rbuf->push(recv_msg);
    
    // フレーム受信を再開
    const auto bind = boost::bind(&UDPFrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred);
    this->sock.async_receive_from(_asio::buffer(this->stream_buf), this->endpoint, bind); 
}

