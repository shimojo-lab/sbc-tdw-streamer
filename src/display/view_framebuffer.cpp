/**********************************************
*            view_framebuffer.cpp             *
*  (framebuffer to put decoded video frames)  *
**********************************************/

#include "view_framebuffer.hpp"

/* constructor (allocate the buffer) */
ViewFramebuffer::ViewFramebuffer(const int width, const int height, const int page_num):
    page_num(page_num),
    page_ptrs(page_num),
    page_states(page_num)
{
    const int frame_size = width * height * COLOR_CHANNEL_NUM;
    for(int i=0; i<this->page_num; ++i){
        this->page_ptrs[i] = new unsigned char[frame_size];
        this->page_states[i].store(false, std::memory_order_release);
    }
}

/* destructor (release the buffer) */
ViewFramebuffer::~ViewFramebuffer(){
    for(int i=0; i<this->page_num; ++i){
        delete[] this->page_ptrs[i];
    }
}

/* get a domain to put a new frame */
unsigned char *ViewFramebuffer::getDrawPage(const int id){
    while(this->page_states[id].load(std::memory_order_acquire));
    return this->page_ptrs[id];
}

/* get a domain on which the next frame is put */
const unsigned char *ViewFramebuffer::getDisplayPage(){
    while(!this->page_states[this->cur_page].load(std::memory_order_acquire)){
        std::this_thread::sleep_for(_chrono::nanoseconds(VIEWBUF_SPINLOCK_INTERVAL));
    }
    return this->page_ptrs[this->cur_page];
}

/* get the value of cur_page */
const int ViewFramebuffer::getCurrentPage(){
    return this->cur_page;
}

/* make a domain displayable to the frame viewer */
void ViewFramebuffer::activatePage(const int id){
    this->page_states[id].store(true, std::memory_order_release);
}

/* make a domain undisplayable to the frame viewer */
void ViewFramebuffer::deactivatePage(){
    this->page_states[this->cur_page].store(false, std::memory_order_release);
    this->cur_page = (this->cur_page+1) % this->page_num;
}

