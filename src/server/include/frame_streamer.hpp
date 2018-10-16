/** frame_streamer.hpp  **/

#ifndef FRAME_STREAMER_HPP
#define FRAME_STREAMER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

namespace asio = boost::asio;

/* 分割フレーム送信器 */
class FrameStreamer{
    private:
        asio::io_service *ios;                // BoostのI/Oオブジェクト
        asio::ip::tcp::socket *sock;          // 送信用TCPソケット
        std::vector<unsigned char> comp_buf;  // 画像圧縮用バッファ        
    public:
        FrameStreamer(asio::io_service *ios, const char *ip, int port);  // コンストラクタ
        ~FrameStreamer();                                     // デストラクタ
        void connectToDisplayNode(const char *ip, int port);  // ディスプレイノードと接続
        void callbackForConnection(const boost::system::error_code &error);  // コールバック
        bool sendFrame(cv::Mat &frame);                       // 分割フレームを送信
};

#endif

