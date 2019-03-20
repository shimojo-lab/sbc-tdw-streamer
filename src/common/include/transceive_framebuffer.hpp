/****************************************
*      transceive_framebuffer.hpp       *
*  (framebuffer used for transmission)  *
****************************************/

#ifndef TRANSCEIVE_FRAMEBUFFER_HPP
#define TRANSCEIVE_FRAMEBUFFER_HPP

#include <mutex>
#include <thread>
#include <chrono>
#include <boost/circular_buffer.hpp>

const int BUF_FRONT_INDEX = 0;            // the index of the buffer head
const int TRANBUF_SPINLOCK_INTERVAL = 1;  // the interval time in the spinlock

/* framebuffer used for transmission */
class TransceiveFramebuffer{
    private:
        boost::circular_buffer<std::string> jpeg_buf;  // the buffer
        std::mutex lock;                               // the mutex lock
    
    public:
        TransceiveFramebuffer(const int jpegbuf_num);  // consructor
        void push(const std::string& jpeg_frame);      // push a JPEG frame
        const std::string pop();                       // pop a JPEG frame
        const int getStoredNum();                      // get the number of stored JPEG frames
};

using tranbuf_ptr_t = std::shared_ptr<TransceiveFramebuffer>;

#endif  /* TRANSCEIVE_FRAMEBUFFER_HPP */

