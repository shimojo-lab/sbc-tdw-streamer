/***************************************
*          display_client.hpp          *
*  (the class for the display client)  *
***************************************/

#ifndef DISPLAY_CLIENT_HPP
#define DISPLAY_CLIENT_HPP

#include "config_parser.hpp"
#include "frame_receiver.hpp"
#include "frame_decoder.hpp"
#include "frame_viewer.hpp"

using init_params_t = std::tuple<
    int, int, int, int, int, int, int, int, int
>;

/* the class for the display client */
class DisplayClient{
    private:
        _asio::io_service& ios;              // the I/O event loop
        _ip::tcp::socket sock;               // the TCP socket
        _asio::streambuf stream_buf;         // the streambuffer
        std::string ip_addr;                 // the IP address of the head node
        std::string fb_dev;                  // the device file of fbdev
        std::thread recv_thre;               // the receiver thread
        std::vector<std::thread> dec_thres;  // the decoder threads
        
        const init_params_t parseInitMsg(const std::string& msg);  // parse the initial message
        void onConnect(const err_t& err);                          // the callback when connecting to the head node
        void onRecvInitMsg(const err_t& err, size_t t_bytes);      // the callback when receving the initial message
        void runFrameReceiver(const int stream_port,               // launch the frame receiver
                              const tranbuf_ptr_t recv_buf);
        void runFrameDecoder(const tranbuf_ptr_t recv_buf,         // launch the frame decoder
                             const viewbuf_ptr_t view_buf);
    
    public:
        DisplayClient(_asio::io_service& ios, ConfigParser& parser);  // constructor
};

#endif  /* DISPLAY_CLIENT_HPP */

