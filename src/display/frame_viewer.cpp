/**************************
 *    frame_viewer.cpp    *
 *    (フレーム表示器)    *
 *************************/

#include "frame_viewer.hpp"

/* コンストラクタ */
FrameViewer::FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock, const jpegbuf_ptr_t recv_buf, 
                         const rawbuf_ptr_t view_buf):
    ios(ios),
    sock(sock),
    recv_buf(recv_buf),
    view_buf(view_buf)
{
    // フレーム表示を開始
    this->sendSync();
    ios.run();
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
        print_err("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージを取得
    const auto data = this->stream_buf.data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    const int iter = recv_msg.length() - MSG_DELIMITER_LEN;
    recv_msg.erase(iter);
    this->stream_buf.consume(t_bytes);
    
    // フレームを表示
    this->view_buf->swapFramebuffer();
    
    // 次番フレームの表示を開始
    this->view_buf->waitForFramebuffer();
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

