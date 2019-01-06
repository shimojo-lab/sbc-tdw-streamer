/***************************
*    frame_receiver.cpp    *
*     (フレーム受信器)     *
***************************/

#include "frame_receiver.hpp"

/* コンストラクタ */
FrameReceiver::FrameReceiver(_asio::io_service& ios, const std::string ip, const int port,
                             const jpegbuf_ptr_t rbuf):
    ios(ios),
    sock(ios),
    rbuf(rbuf)
{
    // フレーム受信を開始
    print_info("Receiving video frames from " + ip + ":" + std::to_string(port));
    this->run(ip, port);
}

/* フレーム受信を開始 */
void FrameReceiver::run(const std::string ip, const int port){
    this->sock.async_connect(_ip::tcp::endpoint(_ip::address::from_string(ip), port),
                             boost::bind(&FrameReceiver::onConnect, this, _ph::error)
    );
    ios.run(); 
}

/* 接続時のコールバック */
void FrameReceiver::onConnect(const err_t& err){
    if(err){
        print_err("Failed TCP connection with head node", err.message());
        return;
    }
    
    // フレーム受信を開始
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred)
    );
}

/* フレーム受信時のコールバック */
void FrameReceiver::onRecvFrame(const err_t& err, size_t t_bytes){
    if(err){
        print_warn("Failed to receive frame", err.message());
    }else{
        // フレームを取得
        const auto data = this->stream_buf.data();
        std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
        const int iter = recv_msg.length() - MSG_DELIMITER_LEN;
        recv_msg.erase(iter);
        this->rbuf->push(recv_msg);
        this->stream_buf.consume(t_bytes);
    }
    
    // フレーム受信を再開
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred)
    ); 
}

