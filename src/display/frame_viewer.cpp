/**********************************
 *        frame_viewer.cpp        *
 *  (the viewer of video frames)  *
 *********************************/

#include "frame_viewer.hpp"

/* constructor */
FrameViewer::FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock, 
                         const viewbuf_ptr_t view_buf, const std::string& fb_dev, const int width,
                         const int height, const std::string& tty_dev, SyncMessageGenerator& generator):
    ios(ios),
    sock(sock),
    view_buf(view_buf),
    generator(generator)
{
    // open the framebuffer of fbdev
    if(!this->openFramebuffer(fb_dev, width, height)){
        std::exit(EXIT_FAILURE);
    }
    
    // get the initial frame
    this->pre_t = _chrono::high_resolution_clock::now();
    this->next_frame = this->view_buf->getDisplayPage();
    this->post_t = _chrono::high_resolution_clock::now();
    this->generator.wait_t_sum += _chrono::duration_cast<_chrono::milliseconds>(this->post_t-this->pre_t).count();
    
    // send a sync message
    this->pre_t = _chrono::high_resolution_clock::now();
    this->sendSync();
    ios.run();
}

/* destructor */
FrameViewer::~FrameViewer(){
    munmap(this->fb_ptr, this->fb_size);
    ioctl(this->tty, KDSETMODE, KD_TEXT);
    close(this->fb);
    close(this->tty);
}

/* open the framebuffer of fbdev */
const bool FrameViewer::openFramebuffer(const std::string& fb_dev, const int width, const int height){
    // load the device file of fbdev
    this->fb = open(fb_dev.c_str(), O_RDWR);
    if(this->fb == DEVICE_OPEN_FAILED){
        _ml::caution("Failed to open framebuffer", fb_dev);
        return false;
    }
    
    // set the size of the framebuffer
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    if(ioctl(this->fb, FBIOGET_VSCREENINFO, &vinfo)){
        _ml::caution("Could not get framebuffer info", "ioctl failed");
        return false;
    }
    vinfo.bits_per_pixel = BITS_PER_PIXEL;
    vinfo.xres = width;
    vinfo.yres = height;
    vinfo.xres_virtual = vinfo.xres;
    vinfo.yres_virtual = vinfo.yres;
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
    if(ioctl(this->fb, FBIOPUT_VSCREENINFO, &vinfo)){
        _ml::caution("Could not set framebuffer size", "ioctl failed");
        return false;
    }
    if(ioctl(this->fb, FBIOGET_FSCREENINFO, &finfo)){
        _ml::caution("Could not get framebuffer info", "ioctl failed");
        return false;
    }
    this->fb_size = vinfo.yres * finfo.line_length;
    
    // map the framebuffer onto the memory
    this->fb_ptr = (unsigned char*)mmap(NULL,
                                        this->fb_size,
                                        PROT_READ|PROT_WRITE,
                                        MAP_SHARED,
                                        this->fb,
                                        0
    );
    if(fb_ptr == MAP_FAILED){
        _ml::caution("Failed to open framebuffer", "mmap falied");
        return false;
    }
    return true;
}

/* display a frame */
void FrameViewer::displayFrame(){
    std::memcpy(this->fb_ptr, this->next_frame, this->fb_size);
    msync(this->fb_ptr, this->fb_size, MS_SYNC|MS_INVALIDATE);
    std::this_thread::sleep_for(std::chrono::milliseconds(DISPLAY_INTERVAL));
}

/* send a sync message */
void FrameViewer::sendSync(){
    const std::string send_msg = this->generator.generate() + MSG_DELIMITER;
    _asio::async_write(this->sock,
                       _asio::buffer(send_msg),
                       boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred)
    );
}

/* the callback when sending a sync message */
void FrameViewer::onSendSync(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Could not send sync message", err.message());
        return;
    }
    
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameViewer::onRecvSync, this, _ph::error, _ph::bytes_transferred)
    );
}

/* the callback when receiving a sync message */
void FrameViewer::onRecvSync(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    this->post_t = _chrono::high_resolution_clock::now();
    this->generator.sync_t_sum += _chrono::duration_cast<_chrono::milliseconds>(this->post_t-this->pre_t).count();
    
    // display a frame
    this->pre_t = _chrono::high_resolution_clock::now();
    this->displayFrame();
    this->view_buf->deactivatePage();
    this->post_t = _chrono::high_resolution_clock::now();
    this->generator.view_t_sum += _chrono::duration_cast<_chrono::milliseconds>(this->post_t-this->pre_t).count();
    
    // get a next frame
    this->pre_t = _chrono::high_resolution_clock::now();
    this->next_frame = this->view_buf->getDisplayPage();
    this->post_t = _chrono::high_resolution_clock::now();
    this->generator.wait_t_sum += _chrono::duration_cast<_chrono::milliseconds>(this->post_t-this->pre_t).count();
    
    // send a sync message
    this->pre_t = _chrono::high_resolution_clock::now();
    this->sendSync();
}

