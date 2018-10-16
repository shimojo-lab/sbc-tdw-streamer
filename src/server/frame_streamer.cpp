/** frame_streamer.cpp **/

#include "frame_streamer.hpp"

// コンストラクタ
FrameStreamer::FrameStreamer(asio::io_service *ios, const char *ip, int port){
    // パラメータを設定
    this->ios = ios;
    
    // ディスプレイノードと接続
    this->connectToDisplayNode(ip, port);
}

// デストラクタ
FrameStreamer::~FrameStreamer(){
    
}

// ディスプレイノードにTCP接続するメソッド
void FrameStreamer::connectToDisplayNode(const char *ip, int port){
    // 送信用TCPソケットを用意
    asio::ip::tcp::socket sock(*(this->ios));
    this->sock = &sock;
    
    // ディスプレイノードに接続
    bool result = true;
    this->sock->async_connect(
        asio::ip::tcp::endpoint(asio::ip::address::from_string(ip), port),
        boost::bind(&FrameStreamer::callbackForConnection, this, asio::placeholders::error)
    );
    return;
}

// TCP接続時のコールバック
void FrameStreamer::callbackForConnection(const boost::system::error_code &error){
    if(error){
        std::cerr << "[Error] Connection failed. (" << error.message() << ")" << std::endl;
    }
    return;
}

// 分割フレームを送信するメソッド
inline bool FrameStreamer::sendFrame(cv::Mat &frame){
    // フレームを圧縮
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", frame, this->comp_buf, params);
    
    // 送信用バイト列を作成
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += "\r\n\r\n";
    auto send_buf = asio::buffer(bytes_buf);
    
    // フレームを送信
    bool result = true;
    //asio::write(*(this->sock), send_buf, this->error);
    /*if(this->error){
        std::cout << "[Error] Send failed. (" << error.message() << ")" << std::endl;
        result = false;
    }*/
    return result;
}

