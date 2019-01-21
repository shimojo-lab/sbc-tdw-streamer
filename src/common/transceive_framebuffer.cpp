/*********************************
*   transceive_framebuffer.hpp   *
*    (送受信フレームバッファ)    *
*********************************/

#include "transceive_framebuffer.hpp"

/* コンストラクタ */
TransceiveFramebuffer::TransceiveFramebuffer(const int jpegbuf_num):
    jpeg_buf(jpegbuf_num)
{}

/* フレームを格納 */
void TransceiveFramebuffer::push(const std::string& jpeg_frame){
    while(this->jpeg_buf.full()){
        std::this_thread::sleep_for(std::chrono::nanoseconds(TRANBUF_SPINLOCK_INTERVAL));
    }
    this->jpeg_buf.push_back(jpeg_frame);
}

/* フレームを取り出し */
const std::string TransceiveFramebuffer::pop(){
    std::lock_guard<std::mutex> pop_lock(this->lock);
    while(this->jpeg_buf.empty()){
        std::this_thread::sleep_for(std::chrono::nanoseconds(TRANBUF_SPINLOCK_INTERVAL));
    }
    const std::string jpeg_frame = this->jpeg_buf[BUF_FRONT_INDEX];
    this->jpeg_buf.pop_front();
    return jpeg_frame;
}

/* 使用中の領域数数を取得 */
const int TransceiveFramebuffer::getStoredNum(){
    return this->jpeg_buf.size();
}

