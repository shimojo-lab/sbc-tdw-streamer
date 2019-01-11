/**************************
 *    frame_viewer.cpp    *
 *    (フレーム表示器)    *
 *************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock, const tranbuf_ptr_t recv_buf, 
                         const rawbuf_ptr_t view_buf, const std::string fb_dev, const int width,
                         const int height, const std::string tty_dev):
    ios(ios),
    sock(sock),
    recv_buf(recv_buf),
    view_buf(view_buf)
{
    // フレームバッファをオープン
    if(!this->openFramebuffer(fb_dev, width, height)){
        std::exit(EXIT_FAILURE);
    }
    
    // カーソルを非表示化
    this->hideCursor(tty_dev);
    
    // フレーム表示を開始
    this->next_frame = this->view_buf->getDisplayArea();
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
const bool FrameViewer::hideCursor(const std::string& tty_dev){
    this->tty = open(tty_dev.c_str(), O_WRONLY);
    if(this->tty == DEVICE_OPEN_FAILED){
        _ml::caution("Could not open tty", tty_dev);
        return false;
    }
    else if(ioctl(this->tty, KDSETMODE, KD_GRAPHICS)){
        _ml::caution("Could not hide cursor", "ioctl failed");
        return false;
    }
    return true;
}

/* フレームを表示 */
void FrameViewer::displayFrame(){
    try{
        std::memcpy(this->fb_ptr, this->next_frame, this->fb_size);
        msync(this->fb_ptr, this->fb_size, MS_SYNC);
    }catch(...){
        _ml::warn("Failed to display frame", "unable to write on framebuffer");
    }
    this->view_buf->subFrameNum();
}

/* 同期メッセージを送信 */
void FrameViewer::sendSync(){
    // メッセージを送信
    const std::string send_msg = MSG_DELIMITER;
    _asio::async_write(this->sock,
                       _asio::buffer(send_msg),
                       boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred)
    );
}

/* 同期メッセージ受信時のコールバック */
void FrameViewer::onRecvSync(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージを取得
    const auto data = this->stream_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    const int iter = recv_msg.length() - MSG_DELIMITER_LEN;
    recv_msg.erase(iter);
    this->stream_buf.consume(t_bytes);
    
    // フレームを表示
    this->displayFrame();
    
    // 次番フレーム用の同期を開始
    this->next_frame = this->view_buf->getDisplayArea();
    this->sendSync();
}

/* 同期メッセージ送信時のコールバック */
void FrameViewer::onSendSync(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Failed to send sync message", err.message());
        return;
    }
    
    // 同期メッセージを受信
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameViewer::onRecvSync, this, _ph::error, _ph::bytes_transferred)
    );
}

