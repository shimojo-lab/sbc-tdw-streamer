/**************************
 *    frame_viewer.cpp    *
 *    (フレーム表示器)    *
 *************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(ios_t& ios, tcp_t::socket& sock, const framebuf_ptr_t vbuf, const double threshold, const std::string fb_dev):
    ios(ios),
    sock(sock),
    vbuf(vbuf),
    mem_checker(threshold)
{
    // フレームバッファをオープン
    if(!this->openFramebuffer(fb_dev)){
        std::exit(EXIT_FAILURE);
    }
    
    // フレーム表示を開始
    this->sendSync();
    ios.run();
}

/* フレームバッファをオープン */
bool FrameViewer::openFramebuffer(const std::string fb_dev){
    // デバイスファイルを読み込み
    this->fb = open(fb_dev.c_str(), O_RDWR);
    if(this->fb < 0){
        print_err("Failed to open framebuffer", fb_dev);
        return false;
    }
    
    // フレームバッファのサイズを取得
    struct fb_var_screeninfo var;
    struct fb_fix_screeninfo fix;
    if(!ioctl(this->fb, FBIOGET_VSCREENINFO, &var) && !ioctl(this->fb, FBIOGET_FSCREENINFO, &fix)){;
        this->fb_len = var.yres_virtual * fix.line_length;
    }else{
        print_err("Failed to get framebuffer size", "ioctl failed");
    }
    
    // フレームバッファをメモリ上にマッピング
    this->fb_ptr = (char*)mmap(NULL, this->fb_len, PROT_READ|PROT_WRITE, MAP_SHARED, this->fd, 0);
    if(fb_ptr == MAP_FAILED){
        print_err("Failed to open framebuffer", "mmap falied");
        return false;
    }else{
        std::atexit(this->closeFramebuffer());
        return true;
    }
}

/* フレームバッファをクローズ */
void FrameViewer::closeFramebuffer(){
    munmap(this->fb_ptr, this->buflen);
    close(this->fd);
}

/* 同期メッセージを送信 */
void FrameViewer::sendSync(){
    // メモリ残量を確認
    std::string send_msg;
    if(this->mem_checker.checkShortage()){
        send_msg += std::to_string(CLI_MEM_SAFE);
    }else{
        send_msg += std::to_string(CLI_MEM_DANGER);
    }
    
    // メッセージを送信
    send_msg += MSG_DELIMITER;
    const auto bind = boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred); 
    _asio::async_write(this->sock, _asio::buffer(send_msg), bind);
}

/* フレームを表示 */
void FrameViewer::displayFrame(){
    char *cur_fb_ptr = this->fb_ptr;
    cv::Mat frame = this->vbuf->pop();
    cv::Vec3b *src;
    try{
        for(int y=0; y<frame.rows; ++y){
            src = frame.ptr<cv::Vec3b>(y);
            for(int x=0; x<frame.cols; ++x){
                *cur_fb_ptr++ = src[x][0];
                *cur_fb_ptr++ = src[x][1];
                *cur_fb_ptr++ = src[x][2];
                *cur_fb_ptr++ = FB_ALPHA;
            }
        }
    }catch(...){
        print_err("Failed to display frame", "failed to write in framebuffer");
    }
}

/* 同期メッセージ受信時のコールバック */
void FrameViewer::onRecvSync(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージを取得
    const auto data = this->stream_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<MSG_DELIMITER_LEN; ++i){
        recv_msg.pop_back();
    }
    this->stream_buf.consume(t_bytes);
    
    // フレームを表示
    this->displayFrame();
    
    // 次番フレームの表示を開始
    this->sendSync();
}

/* 同期メッセージ送信時のコールバック */
void FrameViewer::onSendSync(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to send sync message", err.message());
        return;
    }
    
    // 同期メッセージを受信
    const auto bind = boost::bind(&FrameViewer::onRecvSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->stream_buf, MSG_DELIMITER, bind);
}

