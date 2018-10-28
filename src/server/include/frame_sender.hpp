/*******************************
*       frame_sender.hpp       *
*     (分割フレーム送信器)     *
********************************/

#ifndef FRAME_SENDER_HPP
#define FRAME_SENDER_HPP

#include "frame_queue.hpp"
#include "console_mutex.hpp"
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <opencv2/imgcodecs.hpp>

namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _sys = boost::system;
namespace _ph = boost::asio::placeholders;

extern const char* const SEPARATOR;                    // 受信メッセージのセパレータ
using smt_ios_t = std::shared_ptr<_asio::io_service>;  // io_serviceのsharedポインタ

/* 分割フレーム送信器 */
class FrameSender{
    private:
        const smt_ios_t ios;                  // BoostのI/Oオブジェクト
        _ip::tcp::socket sock;                // TCPソケット
        const smt_fq_t queue;                 // 分割フレーム用キュー
        const char *ip;                       // 送信先のIP
        int port;                             // 送信先のポート
        std::vector<unsigned char> comp_buf;  // フレーム圧縮用バッファ
    public:
        FrameSender(const smt_ios_t ios, const smt_fq_t queue);  // コンストラクタ
        ~FrameSender();                                    // デストラクタ
        void start(const char* const ip, const int port);  // 送信処理を開始
        void onTCPConnect(const _sys::error_code &error);  // TCP接続時のコールバック
        std::string compressFrame(cv::Mat &frame);         // 分割フレームを圧縮
        void onSend(const _sys::error_code &error, std::size_t sended_bytes);  // 分割フレーム送信時のコールバック
};

#endif  /* FRAME_SENDER_HPP */

