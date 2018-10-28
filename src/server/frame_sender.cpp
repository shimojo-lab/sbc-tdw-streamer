/********************************
 *       frame_sender.cpp       *
 *     (分割フレーム送信器)     *
 ********************************/

#include "frame_sender.hpp"

const char* const SEPARATOR = "--boundary\r\n";  // 受信メッセージのセパレータ

/* コンストラクタ */
FrameSender::FrameSender(smt_ios_t ios, smt_fq_t queue):
    ios(ios),
    sock(*ios),
    queue(queue)
{}

/* デストラクタ */
FrameSender::~FrameSender(){}

/* 送信処理を開始 */
void FrameSender::start(const char *ip, int port){
    this->ip = ip;
    std::cout << "[Info] Connecting to " << ip << ":" << port << "..." << std::endl;
    const auto endpoint = _ip::tcp::endpoint(_ip::address::from_string(ip), port);
    const auto bind = boost::bind(&FrameSender::onTCPConnect, this, _ph::error);
    this->sock.async_connect(endpoint, bind);
    return;
}

/* TCP接続時のコールバック */
void FrameSender::onTCPConnect(const _sys::error_code &error){
    if(error){
        std::cerr << "[Error] TCP Connection with " << this->ip << " failed. (" << error.message() << ")" << std::endl;
        return;
    }
    std::cout << "[Info] Established TCP Connection with " << this->ip << "." << std::endl;
    
    // 分割フレームの送信を開始
    cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&FrameSender::onSend, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(this->compressFrame(frame)), bind);
    return;
}

/* 分割フレームを圧縮 */
std::string FrameSender::compressFrame(cv::Mat &frame){
    // フレームを圧縮
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", frame, this->comp_buf, params);
    
    // 送信用バイト列を作成
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += SEPARATOR;
    return bytes_buf;
}

/* 分割フレーム送信時のコールバック */
void FrameSender::onSend(const _sys::error_code &error, std::size_t bytes_transferred){
    if(error){
        std::cout << "[Error] Send failed. (" << error.message() << ")" << std::endl;
    }
    
    // 次番のフレームを送信
    cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&FrameSender::onSend, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(this->compressFrame(frame)), bind);
    return;
}

