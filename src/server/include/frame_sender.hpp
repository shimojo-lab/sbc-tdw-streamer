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
        _ip::tcp::socket tcp_sock;            // TCPソケット
        _ip::udp::socket udp_sock;            // UDPソケット
        const smt_fq_t queue;                 // 分割フレーム用キュー
        const char* const ip;                 // 送信先のIP
        const int port;                       // 送信先のポート
        const int protocol;                   // 通信プロトコル (0: TCP, 1: UDP)
        std::vector<unsigned char> comp_buf;  // フレーム圧縮用バッファ
    public:
        FrameSender(const smt_ios_t ios, const smt_fq_t queue, const char *ip, const int port, const int protocol);  // コンストラクタ
        void start();                                                        // 送信処理を開始
        void onConnect(const _sys::error_code &error);                       // TCP接続時のコールバック
        void onInitialSend(const _sys::error_code &error, std::size_t t_bytes);  // 初回メッセージ送信時のコールバック
        void onTCPSend(const _sys::error_code &error, std::size_t t_bytes);      // 分割フレーム送信時のコールバック
        void onUDPSend(const _sys::error_code &error, std::size_t t_bytes);      // 分割フレーム送信時のコールバック
        inline std::string compressFrame(cv::Mat &frame);                    // 分割フレームを圧縮
};

/* 分割フレームを圧縮 */
inline std::string FrameSender::compressFrame(cv::Mat &frame){
    // フレームを圧縮
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imencode(".jpg", frame, this->comp_buf, params);
    
    // 送信用バイト列を作成
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += SEPARATOR;
    return bytes_buf;
}

#endif  /* FRAME_SENDER_HPP */

