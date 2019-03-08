/********************************
*       frame_sender.cpp        *
*  (the sender of jpeg frames)  *
********************************/

#include "frame_sender.hpp"

/* constructor */
FrameSender::FrameSender(_asio::io_service& ios, const int port, const int display_num,
                         std::vector<tranbuf_ptr_t>& send_bufs, const int viewbuf_num):
    ios(ios),
    acc(ios, _ip::tcp::endpoint(_ip::tcp::v4(), port)),
    display_num(display_num),
    viewbuf_num(viewbuf_num),
    send_count(0),
    send_msgs(display_num),
    send_bufs(send_bufs)
{
    // prepare for TCP sockets
    this->sock = std::make_shared<_ip::tcp::socket>(ios);
    
    // start waiting for TCP connection
    _ml::notice("Streaming video frames at :" + std::to_string(port));
    this->run();
}

/* start waiting for TCP connection from the display node */
void FrameSender::run(){
    this->acc.async_accept(*this->sock,
                           boost::bind(&FrameSender::onConnect, this, _ph::error)
    );
    this->ios.run();
}

/* send a JPEG frame */
void FrameSender::sendFrame(){
    for(int i=0; i<this->display_num; ++i){
        this->send_msgs[i] = std::to_string(this->fb_id) + this->send_bufs[i]->pop() + MSG_DELIMITER;
        _asio::async_write(*this->socks[i],
                           _asio::buffer(this->send_msgs[i]),
                           boost::bind(&FrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred)
        );
    }
    this->fb_id = (this->fb_id+1) % this->viewbuf_num;
}

/* the callback when connected by the display node */
void FrameSender::onConnect(const err_t& err){
    const std::string ip_addr = this->sock->remote_endpoint().address().to_string();
    if(err){
        _ml::caution("Failed stream connection with " + ip_addr, err.message());
        std::exit(EXIT_FAILURE);
    }else{
        this->send_count.fetch_add(1, std::memory_order_release);
    }
    
    // prepare for a new TCP socket
    this->socks.push_back(this->sock);
    this->sock = std::make_shared<_ip::tcp::socket>(this->ios);
    
    if(this->send_count.load(std::memory_order_acquire) < this->display_num){
        // restart waiting for TCP connection
        this->acc.async_accept(*this->sock,
                               boost::bind(&FrameSender::onConnect, this, _ph::error)
        );
    }else{
        // start JPEG frame streaming
        this->sock->close();
        this->send_count.store(0, std::memory_order_release);
        this->sendFrame();
    }
}

/* the callback when sending a JPEG frame */
void FrameSender::onSendFrame(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Failed to send frame", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // If all the current send processes are finished, start the next send processes
    this->send_count.fetch_add(1, std::memory_order_release);
    if(this->send_count.load(std::memory_order_acquire) == this->display_num){
        this->send_count.store(0, std::memory_order_release);
        this->sendFrame();
    }
}

