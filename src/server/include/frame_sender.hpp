/*******************************
*       frame_sender.hpp       *
*     (分割フレーム送信器)     *
********************************/

#ifndef FRAME_SENDER_HPP
#define FRAME_SENDER_HPP

#include "frame_queue.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <opencv2/imgcodecs.hpp>

/* 定数の定義 */
extern const char* const SEPARATOR;  // 受信メッセージのセパレータ

/* 名前空間のエイリアス */
namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _sys = boost::system;
namespace _ph = boost::asio::placeholders;

/* 分割フレーム送信器 */
class FrameSender{
    private:
        _asio::io_service &ios;               // BoostのI/Oオブジェクト
        _ip::tcp::socket sock;                // TCPソケット
        std::shared_ptr<FrameQueue> queue;    // 分割フレーム用キュー
        const char* ip;                       // 送信先のIP
        std::vector<unsigned char> comp_buf;  // 画像圧縮用バッファ
    public:
        FrameSender(_asio::io_service &ios, std::shared_ptr<FrameQueue> queue);  // コンストラクタ
        ~FrameSender();                                          // デストラクタ
        void start(const char *ip, int port);                    // 送信処理を開始
        void onTCPConnect(const _sys::error_code &error);        // TCP接続時のコールバック
        std::string compressFrame(cv::Mat &frame);               // 分割フレームを圧縮
        void onSend(const _sys::error_code &error, std::size_t bytes_transferred);  // 分割フレーム送信時のコールバック
};

#endif  /* FRAME_SENDER_HPP */

