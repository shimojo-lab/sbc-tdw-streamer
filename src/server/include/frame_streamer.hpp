/** frame_streamer.hpp  **/

#ifndef FRAME_STREAMER_HPP
#define FRAME_STREAMER_HPP

#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

namespace asio = boost::asio;

/* 分割フレーム送信器 */
class FrameStreamer{
    private:
        asio::ip::tcp::socket *sock;      // TCPソケット
        boost::system::error_code error;  // Boostのエラーコード
        std::vector<uchar> comp_buf;      // 画像圧縮用バッファ
        
    public:
        FrameStreamer(const char *ip, int port);              // コンストラクタ
        ~FrameStreamer();                                     // デストラクタ
        bool connectToDisplayNode(const char *ip, int port);  // ディスプレイノードと接続
        bool sendFrame(cv::Mat &frame);                       // 分割フレームを送信
};

#endif

