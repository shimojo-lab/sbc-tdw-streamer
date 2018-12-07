/**************************
 *    frame_viewer.cpp    *
 *    (フレーム表示器)    *
 *************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(ios_t& ios, tcp_t::socket& sock, const framebuf_ptr_t vbuf, const int res_x, const int res_y):
    ios(ios),
    sock(sock),
    vbuf(vbuf)
{
    // フレーム表示を開始
    this->sendSync();
    ios.run();
}

/* 同期メッセージを送信 */
void FrameViewer::sendSync(){
    std::string send_msg("sync");
    send_msg += MSG_DELIMITER;
    const auto bind = boost::bind(&FrameViewer::onSendSync, this, _ph::error, _ph::bytes_transferred); 
    _asio::async_write(this->sock, _asio::buffer(send_msg), bind);
}

/* フレームを表示 */
void FrameViewer::displayFrame(){
    try{
        cv::imshow("", this->vbuf->pop());
        cv::waitKey(5);
    }catch(...){
        print_err("Failed to display frame", "JPEG format error");
    }
}

/* 同期メッセージ受信時のコールバック */
void FrameViewer::onRecvSync(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージを取得
    const auto data = this->recv_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<MSG_DELIMITER_LEN; ++i){
        recv_msg.pop_back();
    }
    this->recv_buf.consume(t_bytes);
    
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
    _asio::async_read_until(this->sock, this->recv_buf, MSG_DELIMITER, bind);
}

