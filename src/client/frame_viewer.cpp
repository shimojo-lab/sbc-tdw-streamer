/******************************
 *      frame_viewer.cpp      *
 *    (分割フレーム表示器)    *
 ******************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(const tcps_ptr_t tcp_sock, const fq_ptr_t queue, const int res_x, const int res_y, const int width, const int height, const int framerate, const int frame_num, boost::barrier& barrier):
    tcp_sock(tcp_sock),
    queue(queue),
    sdl2(res_x, res_y, width, height, framerate),
    width(width),
    height(height),
    frame_num(frame_num),
    barrier(barrier)
{}

/* 同期メッセージを送信 */
void FrameViewer::sendSync(){
    std::string bytes_buf("sync");
    bytes_buf += SEPARATOR;
    const auto bind = boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(*this->tcp_sock, _asio::buffer(bytes_buf), bind);
    return;
}

/* 同期メッセージ受信時のコールバック */
void FrameViewer::onRecvSync(const _sys::error_code& err, std::size_t t_bytes){
    if(err){
        print_err("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージを取得
    const auto data = this->recv_buf.data();
    std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<SEPARATOR_LEN; ++i){
        bytes_buf.pop_back();
    }
    this->recv_buf.consume(t_bytes);
    
    // バリアを解除
    this->barrier.wait();
    return;
}

/* 同期メッセージ送信時のコールバック */
void FrameViewer::onSendSync(const _sys::error_code& err, std::size_t t_bytes){
    if(err){
        print_err("Failed to receive sync message", err.message());
        this->sendSync();
    }else{
        this->barrier.wait();
    }
    return;
}

/* フレームを展開 */
cv::Mat FrameViewer::decompressFrame(){
    cv::Mat frame = cv::imdecode(this->queue->dequeue(), CV_LOAD_IMAGE_UNCHANGED);
    return frame;
}

/* フレームを表示 */
void FrameViewer::displayFrame(const cv::Mat& frame){
    // ディスプレイ間で同期
    this->sendSync();
    
    // フレームを表示
    try{
        cv::imshow("", frame);
        cv::waitKey(10);
    }catch(...){
        std::cerr << "jpeg failed." << std::endl;
    }
    return;
}

/* フレーム表示を開始 */
void FrameViewer::run(){
    for(int i=0; i<this->frame_num; ++i){
        this->displayFrame(this->decompressFrame());
    }
    return;
}

