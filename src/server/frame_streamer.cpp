/********************************
 *      frame_streamer.cpp     *
 *   (フレーム送信モジュール)  *
 *******************************/

#include "frame_streamer.hpp"

/* コンストラクタ */
FrameStreamer::FrameStreamer(_asio::io_service &ios, VideoDemuxer &demuxer, const char *ip, int port, int id):
    ios(ios),
    sock(ios),
    ip(ip),
    demuxer(demuxer),
    id(id)
{
    // 受信側と接続
    this->connectByTCP(ip, port);
}

/* デストラクタ */
FrameStreamer::~FrameStreamer(){}

/* 受信側にTCPで接続するメソッド */
void FrameStreamer::connectByTCP(const char *ip, int port){
    auto bind = boost::bind(&FrameStreamer::onConnect, this, _ph::error);
    this->sock.async_connect(_ip::tcp::endpoint(_ip::address::from_string(ip), port), bind);
    return;
}

/* 接続時のコールバック関数 */
void FrameStreamer::onConnect(const _system::error_code &error){
    if(error){
        std::cerr << "[Error] Connection failed. (" << error.message() << ")" << std::endl;
        return;
    }
    std::cout << "[Info] Established TCP Connection with " << this->ip << "." << std::endl;
    
    // 送信処理をループ
    while(true){
        this->demuxer.divideNextFrame();
        this->sendFrame(this->demuxer.getDividedFrame(this->id));
    }
    return;
}

/* 分割フレームを送信するメソッド */
inline void FrameStreamer::sendFrame(cv::Mat *frame){
    // フレームを圧縮
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", *frame, this->comp_buf, params);
    
    // 送信用バイト列を作成
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += SEPARATOR;
    auto send_buf = _asio::buffer(bytes_buf);
    std::cout << "aaaaa";
    // フレームを送信
    //_asio::write(*(this->sock), send_buf, this->error);
    /*if(this->error){
        std::cout << "[Error] Send failed. (" << error.message() << ")" << std::endl;
    }*/
    return;
}

