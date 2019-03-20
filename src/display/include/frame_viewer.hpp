/*******************************
*       frame_viewer.hpp       *
*   (viewer of video frames)   *
*******************************/

#ifndef FRAME_VIEWER_HPP
#define FRAME_VIEWER_HPP

#include "mutex_logger.hpp"
#include "socket_utils.hpp"
#include "sync_message_generator.hpp"
#include "view_framebuffer.hpp"
#include <cstring>
extern "C"{
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/mman.h>
    #include <linux/fb.h>
    #include <linux/kd.h>
}

const int DEVICE_OPEN_FAILED = -1;                         // the return value in failing the device file
const int BITS_PER_PIXEL = 24;                             // the number of bits per pixel
constexpr double MAX_FPS = 60.0;                           // the maximum frame rate
constexpr int DISPLAY_INTERVAL = (int)(1000.0 / MAX_FPS);  // the interval after displaying a frame

/* viewer of video frames */
class FrameViewer{
    private:
        _asio::io_service& ios;           // the I/O event loop
        _ip::tcp::socket& sock;           // the TCP socket
        _asio::streambuf stream_buf;      // the streambuffer
        const viewbuf_ptr_t view_buf;     // the view framebuffer
        int fb;                           // the device file of fbdev
        int fb_size;                      // the size of the framebuffer of fbdev
        unsigned char *fb_ptr;            // the address of the framebuffer of fbdev
        SyncMessageGenerator& generator;  // the sync message generator
        const unsigned char *next_frame;  // a next frame
        hr_clock_t pre_t;                 // the starting time of a tuning term
        hr_clock_t post_t;                // the end time of a tuning term
        
        const bool openFramebuffer(const std::string& fb_dev,  // open the framebuffer of fbdev
                                   const int width, const int height);
        void displayFrame();                                   // display a frame
        void sendSync();                                       // send a sync message
        void onSendSync(const err_t& err, size_t t_bytes);     // the callback when sending a sync message
        void onRecvSync(const err_t& err, size_t t_bytes);     // the callback when receving a sync message
    
    public:
        FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock,  // constructor
                    const viewbuf_ptr_t view_buf, const std::string& fb_dev,
                    const int width, const int height, SyncMessageGenerator& generator);
        ~FrameViewer();  // destructor
};

#endif  /* FRAME_VIEWER_HPP */

