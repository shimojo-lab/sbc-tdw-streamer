/*****************************
*    view_framebuffer.cpp    *
*   (表示フレームバッファ)   *
*****************************/

#include "view_framebuffer.hpp"

/* コンストラクタ */
ViewFramebuffer::ViewFramebuffer(const int width, const int height, const int viewbuf_num,
                                 const unsigned int wait_usec):
    viewbuf_num(viewbuf_num),
    frame_size(width*height*COLOR_CHANNEL_NUM),
    wait_usec(wait_usec)
{
    // フレームバッファ領域を確保
    for(int i=0; i<this->viewbuf_num; ++i){
        this->viewbuf_ptrs.push_back(new unsigned char[this->frame_size]);
    }
    this->stored_num.store(0, std::memory_order_release);
}

/* デストラクタ */
ViewFramebuffer::~ViewFramebuffer(){
    for(int i=0; i<this->viewbuf_num; ++i){
        delete[] this->viewbuf_ptrs[i];
    }
}

/* フレームバッファの描画領域を取得 */
unsigned char *ViewFramebuffer::getDrawArea(const int id){
    while(this->stored_num.load(std::memory_order_acquire) == this->viewbuf_num){
        usleep(this->wait_usec);
    }
    return this->viewbuf_ptrs[id];
}

/* フレームバッファの表示領域を取得 */
const unsigned char *ViewFramebuffer::getDisplayArea(){
    while(this->stored_num.load(std::memory_order_acquire) == 0){
        usleep(this->wait_usec);
    } 
    const unsigned char *ptr = this->viewbuf_ptrs[this->cur_page];
    this->cur_page = (this->cur_page+1) % this->viewbuf_num;
    return ptr;
}

/* フレームバッファの表示領域のインデックスを取得 */
const int ViewFramebuffer::getCurrentPage(){
    return this->cur_page;
}


/* 格納済フレーム数を加算 */
void ViewFramebuffer::addFrameNum(){
    ++this->stored_num;
}

/* 格納済フレーム数を減算 */
void ViewFramebuffer::subFrameNum(){
    --this->stored_num;
}

