/**************************
 *    frame_viewer.cpp    *
 *    (フレーム表示器)    *
 *************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock, const ucharbuf_ptr_t vbuf,
                         const double threshold, const std::string fb_dev):
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

/* デストラクタ */
FrameViewer::~FrameViewer(){
    munmap(this->fb_ptr, this->fb_len);
    close(this->fb);
}

/* フレームバッファをオープン */
const bool FrameViewer::openFramebuffer(const std::string fb_dev){
    // デバイスファイルを読み込み
    this->fb = open(fb_dev.c_str(), O_RDWR);
    if(this->fb < 0){
        print_err("Failed to open framebuffer", fb_dev);
        return false;
    }
    
    // フレームバッファのサイズを取得
    struct fb_var_screeninfo var;
    struct fb_fix_screeninfo fix;
    if(!ioctl(this->fb, FBIOGET_VSCREENINFO, &var) && !ioctl(this->fb, FBIOGET_FSCREENINFO, &fix)){
        this->fb_len = var.yres_virtual * fix.line_length;
    }else{
        print_err("Failed to get framebuffer size", "ioctl failed");
        return false;
    }
    
    // フレームバッファをメモリ上にマッピング
    this->fb_ptr = (unsigned char*)mmap(NULL,
                                        this->fb_len,
                                        PROT_READ|PROT_WRITE,
                                        MAP_SHARED,
                                        this->fb,
                                        0
    );
    if(fb_ptr == MAP_FAILED){
        print_err("Failed to open framebuffer", "mmap falied");
        return false;
    }
    return true;
}

/* フレームバッファをクリア */
void FrameViewer::clearFrame(){
    try{
        std::memset(this->fb_ptr, DEF_COLOR_VALUE, this->fb_len);
        msync(this->fb_ptr, this->fb_len, MS_INVALIDATE);
    }catch(...){
        print_err("Failed to clear frame", "unable to write on framebuffer");
        std::exit(EXIT_FAILURE);
    }
}

/* フレームを表示 */
void FrameViewer::displayFrame(){
    unsigned char *raw_frame = this->vbuf->pop();
    try{
        std::memcpy(this->fb_ptr, (const char*)raw_frame, this->fb_len);
        msync(this->fb_ptr, this->fb_len, MS_INVALIDATE);
    }catch(...){
        print_warn("Failed to display frame", "unable to write on framebuffer");
    }
    delete[] raw_frame;
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
    _asio::async_write(this->sock,
                       _asio::buffer(send_msg),
                       boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred)
    );
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
    _asio::async_read_until(this->sock,
                            this->stream_buf,
                            MSG_DELIMITER,
                            boost::bind(&FrameViewer::onRecvSync, this, _ph::error, _ph::bytes_transferred)
    );
}

