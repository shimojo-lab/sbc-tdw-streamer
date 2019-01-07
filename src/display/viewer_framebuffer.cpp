/*****************************
*   viewer_framebuffer.cpp   *
*   (表示フレームバッファ)   *
*****************************/

#include "viewer_framebuffer.hpp"

/* コンストラクタ */
ViewerFramebuffer::ViewerFramebuffer(const int width, const int height, const int viewbuf_num):
    viewbuf_num(viewbuf_num),
    viewbuf_size(width*height*COLOR_CHANNEL_NUM*this->viewbuf_num)
{
    // バッファ領域を確保
    this->viewbuf_ptr = new unsigned char[this->viewbuf_size];
    this->stored_num.store(0);
}

/* デストラクタ */
ViewerFramebuffer::~ViewerFramebuffer(){
    delete[] this->viewbuf_ptr;
}

/* フレームバッファの描画領域を取得 */
unsigned char *ViewerFramebuffer::getDrawArea(const int id){
    while(this->stored_num.load() == this->viewbuf_size){
        usleep(1000);
    }
    unsigned char *draw_area_ptr = this->viewbuf_ptr + id * this->viewbuf_size;
    return draw_area_ptr;
}

/* フレームバッファの表示領域を取得 */
unsigned char *ViewerFramebuffer::getDisplayArea(){
    while(this->stored_num.load() == 0){
        usleep(1000);
    } 
    unsigned char *display_area_ptr = this->viewbuf_ptr + this->cur_page * this->viewbuf_size;
    this->cur_page = (this->cur_page+1) % this->viewbuf_size;
    return display_area_ptr;
}

/* 格納済フレーム数を加算 */
void ViewerFramebuffer::addFrameNum(){
    ++this->stored_num;
}

/* 格納済フレーム数を減算 */
void ViewerFramebuffer::subFrameNum(){
    --this->stored_num;
}

