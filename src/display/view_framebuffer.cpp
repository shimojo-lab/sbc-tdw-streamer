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
    wait_usec(wait_usec),
    viewbuf_ptrs(viewbuf_num),
    viewbuf_states(viewbuf_num)
{
    // フレームバッファ領域を確保
    for(int i=0; i<this->viewbuf_num; ++i){
        this->viewbuf_ptrs[i] = new unsigned char[this->frame_size];
        this->viewbuf_states[i].store(false);
    }
}

/* デストラクタ */
ViewFramebuffer::~ViewFramebuffer(){
    for(int i=0; i<this->viewbuf_num; ++i){
        delete[] this->viewbuf_ptrs[i];
    }
}

/* フレームバッファの描画領域を取得 */
unsigned char *ViewFramebuffer::getDrawArea(const int id){
    while(this->viewbuf_states[id].load());
    return this->viewbuf_ptrs[id];
}

/* フレームバッファの表示領域を取得 */
const unsigned char *ViewFramebuffer::getDisplayArea(){
    while(!this->viewbuf_states[this->cur_page].load());
    return this->viewbuf_ptrs[this->cur_page];
}

/* フレームバッファの表示領域のインデックスを取得 */
const int ViewFramebuffer::getCurrentPage(){
    return this->cur_page;
}

/* フレーム領域の表示を有効化 */
void ViewFramebuffer::activateFrame(const int id){
    this->viewbuf_states[id].store(true);
}

/* フレーム領域の表示を無効化 */
void ViewFramebuffer::deactivateFrame(){
    this->viewbuf_states[this->cur_page].store(false);
    this->cur_page = (this->cur_page+1) % this->viewbuf_num;
}

