/********************************
*      frame_receiver.cpp       *
*   (receiver of JPEG frames)   *
********************************/

#include "frame_receiver.hpp"

/* constructor */
FrameReceiver::FrameReceiver(_asio::io_service& ios, const std::string& ip_addr, const int stream_port,
                             const tranbuf_ptr_t recv_buf):
    ios(ios),
    sock(ios),
    recv_buf(recv_buf)
{
    _ml::notice("Receiving video frames from " + ip_addr + ":" + std::to_string(stream_port));
    this->run(ip_addr, stream_port);
}

/* start receiving JPEG frames */
void FrameReceiver::run(const std::string& ip_addr, const int stream_port){
    this->sock.async_connect(_ip::tcp::endpoint(_ip::address::from_string(ip_addr), stream_port),
                             boost::bind(&FrameReceiver::onConnect, this, _ph::error)
    );
    this->ios.run();
}

/* the callback when connected by the head node */
void FrameReceiver::onConnect(const err_t& err){
    if(err){
        _ml::caution("Failed stream connection with head node", err.message());
        return;
    }
    
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred)
    );
}

/* the callback when receiving a JPEG frame */
void FrameReceiver::onRecvFrame(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Could not receive frame", err.message());
    }else{
        const auto data = this->stream_buf.data();
        std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_end(data));
        recv_msg.erase(recv_msg.length()-MSG_DELIMITER_LEN);
        this->recv_buf->push(recv_msg);
        this->stream_buf.consume(t_bytes);
    }
    
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameReceiver::onRecvFrame, this, _ph::error, _ph::bytes_transferred)
    );
}

