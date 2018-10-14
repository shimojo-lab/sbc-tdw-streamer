/** frame_streamer.cpp **/

#include "frame_streamer.hpp"

// コンストラクタ
FrameStreamer::FrameStreamer(const char *ip, int port){
    // パラメータを設定
    asio::io_service ios;
    asio::ip::tcp::socket sock(ios);
    boost::system::error_code error;
    this->sock = &sock;
    this->error = error;
    
    // ディスプレイノードと接続
    if(this->connectToDisplayNode(ip, port) == false){
        exit(-1);
    }
}

// デストラクタ
FrameStreamer::~FrameStreamer(){
    
}

// ディスプレイノードとコネクションを確立するメソッド
bool FrameStreamer::connectToDisplayNode(const char *ip, int port){
    // ディスプレイノードに接続
    bool result = true;
    this->sock->connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port), this->error);
    if(this->error){
        std::cout << "[Error] Connection failed. (" << error.message() << ")" << std::endl;
        result = false;
    }else{
        std::cout << "Connected to display (" << ip  << ")." << std::endl;
    }
    return result;
}

// 分割フレームを送信するメソッド
inline bool FrameStreamer::sendFrame(cv::Mat &frame){
    // フレームを圧縮
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", frame, this->comp_buf, params);
    
    // フレームを送信
    bool result = true;
    auto send_buf = asio::buffer(this->comp_buf);
    asio::write(*(this->sock), send_buf, this->error);
    if(this->error){
        std::cout << "[Error] Send failed. (" << error.message() << ")" << std::endl;
        result = false;
    }
    return result;
}

