/**************************
 *    frame_viewer.cpp    *
 *    (フレーム表示器)    *
 *************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock, const tranbuf_ptr_t recv_buf, 
                         const viewbuf_ptr_t view_buf, const std::string fb_dev, const int width,
                         const int height, const std::string tty_dev, const int tuning_term):
    ios(ios),
    sock(sock),
    recv_buf(recv_buf),
    view_buf(view_buf),
    tuning_term(tuning_term)
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

/* 同期メッセージを生成 */
const std::string FrameViewer::makeSyncMsg(){
    this->tuning_params.setIntParam("id", this->view_buf->getCurrentPage());
    if(this->frame_count == this->tuning_term){
        this->frame_count = 0;
        this->tuning_params.setIntParam("tune", JPEG_TUNING_ON);
        this->tuning_params.setIntParam("param", JPEG_PARAM_KEEP);
        this->tuning_params.setIntParam("change", JPEG_PARAM_KEEP);
    }else{
        this->tuning_params.setIntParam("tune", JPEG_TUNING_OFF);
    }
    return this->tuning_params.serialize();
}

/* 同期メッセージを送信 */
void FrameViewer::sendSync(){
    // メッセージを送信
    const std::string send_msg = this->makeSyncMsg() + MSG_DELIMITER;
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
    recv_msg.erase(recv_msg.length()-MSG_DELIMITER_LEN);
    this->stream_buf.consume(t_bytes);
    
    // フレームを表示
    this->displayFrame();
    this->view_buf->deactivateFrame();
    ++this->frame_count;
    
    // フレームレートを計算
    const hr_chrono_t post_time = std::chrono::high_resolution_clock::now();
    const double fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(post_time-this->pre_time).count();
    _ml::debug(std::to_string(fps)+"fps");
    this->pre_time = post_time;
    
    // 次番フレーム用の同期を開始
    this->next_frame = this->view_buf->getDisplayArea();
    this->sendSync();
}

/* 同期メッセージ送信時のコールバック */
void FrameViewer::onSendSync(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Could not send sync message", err.message());
        return;
    }
    
    // 同期メッセージを受信
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameViewer::onRecvSync, this, _ph::error, _ph::bytes_transferred)
    );
}

