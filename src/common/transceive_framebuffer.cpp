/*******************************************
*        transceive_framebuffer.hpp        *
*  (the framebuffer used in transmission)  *
*******************************************/

#include "transceive_framebuffer.hpp"

/* constructor (allocate the buffer) */
TransceiveFramebuffer::TransceiveFramebuffer(const int jpegbuf_num):
    jpeg_buf(jpegbuf_num)
{}

/* push a JPEG frame in the buffer */
void TransceiveFramebuffer::push(const std::string& jpeg_frame){
    while(this->jpeg_buf.full()){
        std::this_thread::sleep_for(std::chrono::nanoseconds(TRANBUF_SPINLOCK_INTERVAL));
    }
    this->jpeg_buf.push_back(jpeg_frame);
}

/* pop a JPEG frame from the buffer */
const std::string TransceiveFramebuffer::pop(){
    std::lock_guard<std::mutex> pop_lock(this->lock);
    while(this->jpeg_buf.empty()){
        std::this_thread::sleep_for(std::chrono::nanoseconds(TRANBUF_SPINLOCK_INTERVAL));
    }
    const std::string jpeg_frame = this->jpeg_buf[BUF_FRONT_INDEX];
    this->jpeg_buf.pop_front();
    return jpeg_frame;
}

/* get the number of stored JPEG frames */
const int TransceiveFramebuffer::getStoredNum(){
    return this->jpeg_buf.size();
}

