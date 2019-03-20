/************************************
*        frontend_server.hpp        *
*  (class for the frontend server)  *
************************************/

#ifndef FRONTEND_SERVER_HPP
#define FRONTEND_SERVER_HPP

#include "config_parser.hpp"
#include "frame_encoder.hpp"
#include "frame_sender.hpp"
#include "sync_manager.hpp"
#include <thread>

/* class for the frontend server */
class FrontendServer{
    private:
        _asio::io_service& ios;                // the I/O event loop
        sock_ptr_t sock;                       // the TCP socket
        _ip::tcp::acceptor acc;                // the TCP acceptor
        std::vector<sock_ptr_t> socks;         // the in-use TCP sockets
        int display_num;                       // the number of the displays
        JsonHandler init_params;               // the parameters packed in the initial message
        int connected_num = 0;                 // the number of the connected display nodes
        jpeg_params_t ycbcr_format_list;       // the YCbCr format list for the display nodes
        jpeg_params_t quality_list;            // the quality factor list for the display nodes
        ip_list_t ip_addrs;                    // the IP addresses of the display nodes
        std::vector<tranbuf_ptr_t> send_bufs;  // the send framebuffer
        std::thread send_thre;                 // the sender thread
        std::thread enc_thre;                  // the encoder thread
        
        void waitForConnection();                          // start waiting for TCP connection
        void onConnect(const err_t& err);                  // the callback when connected by the display node
        void onSendInit(const err_t& err,  size_t t_bytes, // the callback when sending the initial message
                        const std::string ip);
        void runFrameEncoder(const std::string video_src,  // launch the frame encoder
                             const int column, const int row, const int bezel_w, const int bezel_h,
                             const int width, const int height);
        void runFrameSender(const int stream_port,         // launch the frame sender
                            const int viewbuf_num);
        void runSyncManager();                             // launch the sync manager
    
    public:
        FrontendServer(_asio::io_service& ios, ConfigParser& parser, const int fs_port);  // constructor
};

#endif  /* FRONTEND_SERVER_HPP */

