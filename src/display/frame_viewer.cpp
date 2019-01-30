/**************************
 *    frame_viewer.cpp    *
 *    (フレーム表示器)    *
 *************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock, 
                         const viewbuf_ptr_t view_buf, const std::string& fb_dev, const int width,
                         const int height, const std::string& tty_dev, SyncMessageGenerator& generator):
    ios(ios),
    sock(sock),
    view_buf(view_buf),
    generator(generator)
{
    // フレームバッファをオープン
    if(!this->openFramebuffer(fb_dev, width, height)){
        std::exit(EXIT_FAILURE);
    }
    
    // カーソルを非表示化
    this->hideCursor(tty_dev);
    
    // 最初のフレームを取得
    #ifdef DEBUG
    this->pre_view_t = _chrono::high_resolution_clock::now();
    #endif
    this->pre_t = _chrono::high_resolution_clock::now();
    this->next_frame = this->view_buf->getDisplayPage();
    this->post_t = _chrono::high_resolution_clock::now();
    this->generator.wait_t_sum += _chrono::duration_cast<_chrono::milliseconds>(this->post_t-this->pre_t).count();
    
    // 同期メッセージを送信
    this->pre_t = _chrono::high_resolution_clock::now();
    this->sendSync();
    ios.run();
}

/* デストラクタ */
FrameViewer::~FrameViewer(){
    munmap(this->fb_ptr, this->fb_size);
    ioctl(this->tty, KDSETMODE, KD_TEXT);
    close(this->fb);
    close(this->tty);
}

/* フレームバッファをオープン */
const bool FrameViewer::openFramebuffer(const std::string& fb_dev, const int width, const int height){
    // デバイスファイルを読み込み
    this->fb = open(fb_dev.c_str(), O_RDWR);
    if(this->fb == DEVICE_OPEN_FAILED){
        _ml::caution("Failed to open framebuffer", fb_dev);
        return false;
    }
    
    // フレームバッファのサイズを設定
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
    
    // フレームバッファをメモリ上にマッピング
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

/* カーソルを非表示化 */
void FrameViewer::hideCursor(const std::string& tty_dev){
    this->tty = open(tty_dev.c_str(), O_WRONLY);
    if(this->tty == DEVICE_OPEN_FAILED){
        _ml::warn("Could not open tty", tty_dev);
    }
    else if(ioctl(this->tty, KDSETMODE, KD_GRAPHICS)){
        _ml::warn("Could not hide cursor", "ioctl failed");
    }
}

/* フレームを表示 */
void FrameViewer::displayFrame(){
    std::memcpy(this->fb_ptr, this->next_frame, this->fb_size);
    msync(this->fb_ptr, this->fb_size, MS_SYNC|MS_INVALIDATE);
    std::this_thread::sleep_for(std::chrono::milliseconds(DISPLAY_INTERVAL));
}

/* 同期メッセージを送信 */
void FrameViewer::sendSync(){
    const std::string send_msg = this->generator.generate() + MSG_DELIMITER;
    _asio::async_write(this->sock,
                       _asio::buffer(send_msg),
                       boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred)
    );
}

/* 同期メッセージを受信 */
void FrameViewer::onSendSync(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Could not send sync message", err.message());
        return;
    }
    
    // 同期メッセージを送信
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameViewer::onRecvSync, this, _ph::error, _ph::bytes_transferred)
    );
}

/* 同期メッセージ受信時のコールバック */
void FrameViewer::onRecvSync(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージをパース
    const auto data = this->stream_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_end(data));
    recv_msg.erase(recv_msg.length()-MSG_DELIMITER_LEN);
    if(std::stoi(recv_msg) == JPEG_PARAM_UP){
        this->generator.tuning_mode = DEC_SPEED_DOWN;
    }
    this->stream_buf.consume(t_bytes);
    this->post_t = _chrono::high_resolution_clock::now();
    this->generator.sync_t_sum += _chrono::duration_cast<_chrono::milliseconds>(this->post_t-this->pre_t).count();
    
    // フレームを表示
    this->pre_t = _chrono::high_resolution_clock::now();
    this->displayFrame();
    this->view_buf->deactivatePage();
    this->post_t = _chrono::high_resolution_clock::now();
    this->generator.view_t_sum += _chrono::duration_cast<_chrono::milliseconds>(this->post_t-this->pre_t).count();

    #ifdef DEBUG
    // フレームレートを算出
    ++this->frame_count;
    const hr_clock_t post_view_t = _chrono::high_resolution_clock::now();
    this->elapsed += _chrono::duration_cast<_chrono::milliseconds>(this->post_view_t-this->pre_view_t).count();
    if(this->elapsed > 1000.0){
        ++this->total_sec;
        std::cout << this->total_sec << "s: " << frame_count << "fps" << std::endl;
        this->elapsed -= 1000.0;
        this->frame_count = 0;
    }
    this->view_start_t = this->view_end_t;
    #endif
    
    // 次番フレームを取得
    this->pre_t = _chrono::high_resolution_clock::now();
    this->next_frame = this->view_buf->getDisplayPage();
    this->post_t = _chrono::high_resolution_clock::now();
    this->generator.wait_t_sum += _chrono::duration_cast<_chrono::milliseconds>(this->post_t-this->pre_t).count();
    
    // 同期メッセージを送信
    this->pre_t = _chrono::high_resolution_clock::now();
    this->sendSync();
}

