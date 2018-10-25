/********************************
 *      frame_streamer.cpp     *
 *   (フレーム送信モジュール)  *
 *******************************/

#include "frame_streamer.hpp"

const char* const SEPARATOR = "--boundary\r\n";  // TCPメッセージのセパレータ

/* コンストラクタ */
FrameStreamer::FrameStreamer(_asio::io_service &ios, VideoDemuxer &demuxer, const char *ip, int port, int id):
    ios(ios),
    sock(ios),
    demuxer(demuxer),
    ip(ip),
    id(id)
{
    // 受信側と接続
    this->connectByTCP(ip, port);
}

/* デストラクタ */
FrameStreamer::~FrameStreamer(){}

/* 受信側にTCP接続するメソッド */
void FrameStreamer::connectByTCP(const char *ip, int port){
    const auto bind = boost::bind(&FrameStreamer::onTCPConnect, this, _ph::error);
    this->sock.async_connect(_ip::tcp::endpoint(_ip::address::from_string(ip), port), bind);
    return;
}

/* TCP接続時のコールバック関数 */
void FrameStreamer::onTCPConnect(const _sys::error_code &error){
    if(error){
        std::cerr << "[Error] TCP Connection failed. (" << error.message() << ")" << std::endl;
        return;
    }
    std::cout << "[Info] Established TCP Connection with " << this->ip << "." << std::endl;
    
    // フレームの送信を開始
    this->demuxer.divideNextFrame();
    this->sendFrame(this->demuxer.getDividedFrame(this->id));
    return;
}

/* 分割フレームを送信するメソッド */
void FrameStreamer::sendFrame(cv::Mat *frame){
    // フレームを圧縮
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", *frame, this->comp_buf, params);
    
    // 送信用バイト列を作成
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += SEPARATOR;
    
    // フレームを送信
    const auto bind = boost::bind(&FrameStreamer::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->sock, _asio::buffer(bytes_buf), bind);
    return;
}

/* フレーム送信時のコールバック関数 */
void FrameStreamer::onSendFrame(const _sys::error_code &error, size_t size){
    if(error){
        // 送信失敗を通知
        std::cout << "[Error] Send failed. (" << error.message() << ")" << std::endl;
    }else{
        // 次番のフレームを送信
        this->demuxer.divideNextFrame();
        this->sendFrame(this->demuxer.getDividedFrame(this->id));
    }
    return;
}

