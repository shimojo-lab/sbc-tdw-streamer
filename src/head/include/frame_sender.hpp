/******************************
*      frame_sender.hpp       *
*   (sender of JPEG frames)   *
******************************/

#ifndef FRAME_SENDER_HPP
#define FRAME_SENDER_HPP

#include "mutex_logger.hpp"
#include "socket_utils.hpp"
#include "transceive_framebuffer.hpp"
#include <vector>
#include <atomic>

/* sender of JPEG frames */
class FrameSender{
    private:
        _asio::io_service& ios;                 // the I/O event loop
        sock_ptr_t sock;                        // the TCP socket
        _ip::tcp::acceptor acc;                 // the TCP acceptor
        std::vector<sock_ptr_t> socks;          // the in-use TCP sockets
        const int display_num;                  // the number of the displays
        int fb_id = 0;                          // the index in the view framebuffer
        const int viewbuf_num;                  // the number of domains in the view framebuffer
        std::atomic_int send_count;             // the number of sended frames
        std::vector<std::string> send_msgs;     // the send message
        std::vector<tranbuf_ptr_t>& send_bufs;  // the send framebuffer
        
        void run();                                          // start waiting for TCP connection
        void sendFrame();                                    // send a JPEG frame
        void onConnect(const err_t& err);                    // the callback when connected by the display node
        void onSendFrame(const err_t& err, size_t t_bytes);  // the callback when sending a frame
    
    public:
        FrameSender(_asio::io_service& ios, const int port,  // constructor
                    const int display_num, std::vector<tranbuf_ptr_t>& send_bufs,
                    const int viewbuf_num);
};

#endif  /* FRAME_SENDER_HPP */

