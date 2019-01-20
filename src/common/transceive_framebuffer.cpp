/*********************************
*   transceive_framebuffer.hpp   *
*   (送受信用フレームバッファ)   *
*********************************/

#include "transceive_framebuffer.hpp"

/* コンストラクタ */
TransceiveFramebuffer::TransceiveFramebuffer(const int buf_num):
    buf(buf_num)
{}

/* フレームを格納 */
void TransceiveFramebuffer::push(const std::string& frame){
    while(this->buf.full());
    this->buf.push_back(frame);
}

/* フレームを取り出し */
const std::string TransceiveFramebuffer::pop(){
    std::lock_guard<std::mutex> pop_lock(this->lock);
    while(this->buf.empty());
    const std::string frame = this->buf[BUF_FRONT_INDEX];
    this->buf.pop_front();
    return frame;
}

/* 使用中の領域数数を取得 */
const int TransceiveFramebuffer::getStoredNum(){
    return this->buf.size();
}

