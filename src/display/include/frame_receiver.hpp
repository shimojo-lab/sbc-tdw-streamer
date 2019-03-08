/**********************************
*        frame_receiver.hpp       *
*  (the receiver of JPEG frames)  *
**********************************/

#ifndef FRAME_RECEIVER_HPP
#define FRAME_RECEIVER_HPP

#include "mutex_logger.hpp"
#include "socket_utils.hpp"
#include "transceive_framebuffer.hpp"

/* the receiver of JPEG frames */
class FrameReceiver{
    private:
        _asio::io_service& ios;        // the I/O event loop
        _ip::tcp::socket sock;         // the TCP socket
        _asio::streambuf stream_buf;   // the streambuffer
        const tranbuf_ptr_t recv_buf;  // the receive framebuffer
        
        void run(const std::string& ip_addr, const int port);  // start receiving frames
        void onConnect(const err_t& err);                      // the callback when connected by the head node
        void onRecvFrame(const err_t& err, size_t t_bytes);    // the callback when receiving a frame
    
    public:
        FrameReceiver(_asio::io_service& ios, const std::string& ip_addr,  // constructor
                      const int stream_port, const tranbuf_ptr_t recv_buf);
};

#endif  /* FRAME_RECEIVER_HPP */

