/*********************************
*     base_frame_sender.cpp      *
*  (フレーム送信器の基底クラス)  *
*********************************/

#include "base_frame_sender.hpp"

/* コンストラクタ */
BaseFrameSender::BaseFrameSender(_ios& ios, const fq_ptr_t queue, const int port, const bar_ptr_t barrier):
    ios(ios),
    strand(ios),
    sock(ios),
    acc(ios, _tcp::endpoint(_tcp::v4(), port)),
    queue(queue),
    barrier(barrier)
{}

/* TCP接続時のコールバック */
void BaseFrameSender::onConnect(const _err& err){}

/* フレーム送信時のコールバック */
void BaseFrameSender::onSendFrame(const _err& err, size_t t_bytes){}

/* TCP接続待機を開始 */
void BaseFrameSender::startConnect(){}

/* 同期メッセージ受信時のコールバック */
void BaseFrameSender::onRecvSync(const _err& err, size_t t_bytes){
    // 全ディスプレイノード間で同期
    if(err){
        print_err("Failed to receive sync message", err.message());
        return;
    }
    this->barrier->wait();
    
    // 同期メッセージを送信
    std::string bytes_buf("sync");
    bytes_buf += SEPARATOR;    
    const auto bind = boost::bind(&BaseFrameSender::onSendSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(bytes_buf), this->strand.wrap(bind));
}

/* 同期メッセージ送信時のコールバック */
void BaseFrameSender::onSendSync(const _err& err, size_t t_bytes){
    // 同期メッセージの受信を再開
    const auto bind = boost::bind(&BaseFrameSender::onRecvSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->sock, this->recv_buf, SEPARATOR, this->strand.wrap(bind));
}

/* フレームを圧縮 */
const std::string BaseFrameSender::compressFrame(const cv::Mat& frame){
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
    cv::imencode(".jpg", frame, this->comp_buf, params);
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += SEPARATOR;
    return bytes_buf;
}

