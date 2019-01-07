/*****************************
*   viewer_framebuffer.cpp   *
*   (表示フレームバッファ)   *
*****************************/

#include "viewer_framebuffer.hpp"

/* コンストラクタ */
ViewerFramebuffer::ViewerFramebuffer(const std::string fb_dev, const int width, const int height,
                                     const int viewbuf_size, const std::string tty_dev):
    viewbuf_size(viewbuf_size)
{
    // フレームバッファをオープン
    if(!this->openFramebuffer(fb_dev, width, height)){
        std::exit(EXIT_FAILURE);
    }
    
    // カーソルを非表示化
    if(!this->hideCursor(tty_dev)){
        std::exit(EXIT_FAILURE);
    }
}

/* デストラクタ */
ViewerFramebuffer::~ViewerFramebuffer(){
    munmap(this->fb_ptr, this->fb_size);
    ioctl(this->tty, KDSETMODE, KD_TEXT);
    close(this->fb);
    close(this->tty);
}

/* フレームバッファをオープン */
const bool ViewerFramebuffer::openFramebuffer(const std::string fb_dev, const int width,
                                              const int height)
{
    // デバイスファイルを読み込み
    this->fb = open(fb_dev.c_str(), O_RDWR);
    if(this->fb == DEVICE_OPEN_FAILED){
        print_err("Could not open framebuffer", fb_dev);
        return false;
    }
    
    // フレームバッファのサイズを設定
    struct fb_fix_screeninfo finfo;
    if(ioctl(this->fb, FBIOGET_VSCREENINFO, &this->vinfo)){
        print_err("Could not get framebuffer info", "ioctl failed");
        return false;
    }
    this->vinfo.bits_per_pixel = BITS_PER_CHANNEL;
    this->vinfo.xres = width;
    this->vinfo.yres = height;
    this->vinfo.xres_virtual = this->vinfo.xres;
    this->vinfo.yres_virtual = this->vinfo.yres * this->viewbuf_size;
    this->vinfo.xoffset = 0;
    this->vinfo.yoffset = 0;
    if(ioctl(this->fb, FBIOPUT_VSCREENINFO, &this->vinfo)){
        print_err("Could not set framebuffer size", "ioctl failed");
        return false;
    }
    if(ioctl(this->fb, FBIOGET_FSCREENINFO, &finfo)){
        print_err("Could not get framebuffer info", "ioctl failed");
        return false;
    }
    this->fb_size = this->vinfo.yres * finfo.line_length;
    this->total_fb_size = finfo.smem_len;
    
    // フレームバッファをメモリ上にマッピング
    this->fb_ptr = (unsigned char*)mmap(NULL,
                                        this->total_fb_size,
                                        PROT_READ|PROT_WRITE,
                                        MAP_SHARED,
                                        this->fb,
                                        0
    );
    if(this->fb_ptr == MAP_FAILED){
        print_err("Could not open framebuffer", "mmap failed");
        return false;
    }
    return true;
}

/* カーソルを非表示化 */
const bool ViewerFramebuffer::hideCursor(const std::string tty_dev){
    this->tty = open(tty_dev.c_str(), O_WRONLY);
    if(this->tty == DEVICE_OPEN_FAILED){
        print_err("Could not open tty", tty_dev);
        return false;
    }
    else if(ioctl(this->tty, KDSETMODE, KD_GRAPHICS)){
        print_err("Could not hide cursor", "ioctl failed");
        return false;
    }
    return true;
}

/* フレームバッファの描画領域を取得 */
unsigned char *ViewerFramebuffer::getFramebuffer(const int id){
    // フレームバッファ満杯時は待機
    while(this->stored_num.load() == this->viewbuf_size){
        usleep(1000);
    }
    
    // 描画領域のポインタを取得
    unsigned char *draw_buf_ptr = this->fb_ptr + id * this->fb_size;
    return draw_buf_ptr;
}

/* フレームバッファを有効化 */
void ViewerFramebuffer::activateFramebuffer(){
    ++this->stored_num;
}

/* フレームが供給されるまで待機 */
void ViewerFramebuffer::waitForFramebuffer(){
    while(this->stored_num.load() == 0){
        usleep(1000);
    } 
}

/* フレームバッファの表示領域を切り替え */
void ViewerFramebuffer::swapFramebuffer(){ 
    // フレームバッファを切り替え
    this->vinfo.yoffset = this->vinfo.yres * this->cur_page;
    ioctl(this->fb, FBIOPAN_DISPLAY, this->vinfo);
    this->cur_page = (this->cur_page+1) % this->viewbuf_size;
    
    // フレームバッファ満杯時のロックを解除
    --this->stored_num;
}

