/******************************
 *      frame_viewer.cpp      *
 *    (分割フレーム表示器)    *
 ******************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(ios_t& ios, tcp_t::socket& sock, const fq_ptr_t queue, const int res_x, const int res_y, const int width, const int height, const int frame_num, const int frame_rate):
    ios(ios),
    sock(sock),
    queue(queue),
    sdl2(res_x, res_y, width, height, frame_rate),
    width(width),
    height(height),
    frame_num(frame_num)
{
    // フレーム表示を開始
    this->frame = cv::imdecode(this->queue->dequeue(), CV_LOAD_IMAGE_UNCHANGED);
    std::string send_msg("sync");
    send_msg += SEPARATOR;
    const auto bind = boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(send_msg), bind);
    ios.run();
}

/* 同期メッセージ受信時のコールバック */
void FrameViewer::onRecvSync(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive sync message", err.message());
        return;
    }
    
    // 同期メッセージを取得
    const auto data = this->recv_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<SEPARATOR_LEN; ++i){
        recv_msg.pop_back();
    }
    this->recv_buf.consume(t_bytes);
    
    // フレームを表示
    try{
        cv::imshow("", this->frame);
        cv::waitKey(33);
    }catch(...){
        print_err("Failed to display frame", "JPEG format error");
    }
    
    // 次番フレームの表示を開始
    this->frame = this->queue->dequeue();
    std::string send_msg("sync");
    send_msg += SEPARATOR;
    const auto bind = boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(send_msg), bind);
}

/* 同期メッセージ送信時のコールバック */
void FrameViewer::onSendSync(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to send sync message", err.message());
        return;
    }
    
    // 同期メッセージを受信
    const auto bind = boost::bind(&FrameViewer::onRecvSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, SEPARATOR, bind);
}

