/***************************
*    frame_receiver.cpp    *
*     (フレーム受信器)     *
***************************/

#include "frame_receiver.hpp"

/* コンストラクタ */
FrameReceiver::FrameReceiver(_asio::io_service& ios, const tranbuf_ptr_t recv_buf):
    ios(ios),
    sock(ios),
    recv_buf(recv_buf)
{}

/* フレーム受信を開始 */
void FrameReceiver::run(const std::string& ip_addr, const int stream_port){
    // ヘッドノードに接続
    err_t err;
    this->sock.connect(_ip::tcp::endpoint(_ip::address::from_string(ip_addr), stream_port), err);
    if(err){
        _ml::caution("Failed stream connection with head node", err.message());
        return;
    }
    
    // フレーム受信を開始
    while(true){
        _asio::read_until(this->sock, this->stream_buf, MSG_DELIMITER, err);
        if(err){
            _ml::caution("Could not receive frame", err.message());
        }else{
            const auto data = this->stream_buf.data();
            std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_end(data));
            const int t_bytes = recv_msg.length();
            recv_msg.erase(recv_msg.length()-MSG_DELIMITER_LEN);
            this->recv_buf->push(recv_msg);
            this->stream_buf.consume(t_bytes);
        }
    }
}

