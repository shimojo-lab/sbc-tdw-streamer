/*******************************
*   display_synchronizer.hpp   *
*   (ディスプレイ同期制御器)   *
********************************/

#ifndef DISPLAY_SYNCHRONIZER_HPP
#define DISPLAY_SYNCHRONIZER_HPP

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

extern const char* const SEPARATOR;                    // 同期メッセージのセパレータ
using smt_ios_t = std::shared_ptr<_asio::io_service>;  // io_serviceのsharedポインタ

/* ディスプレイ同期制御器 */
class DisplaySynchronizer{
    private:
        const smt_ios_t ios;                  // BoostのI/Oオブジェクト
        _ip::tcp::socket tcp_sock;            // TCPソケット
        const int port;                       // 通信用ポート
    public:
        DisplaySynchronizer(const smt_ios_t ios, const char *ip, const int port);  // コンストラクタ
        void start();                                                        // 送信処理を開始
        void onConnect(const _sys::error_code &error);                       // TCP接続時のコールバック
        void onInitialSend(const _sys::error_code &error, std::size_t t_bytes);  // 初回メッセージ送信時のコールバック
        void onTCPSend(const _sys::error_code &error, std::size_t t_bytes);      // 分割フレーム送信時のコールバック
        inline std::string compressFrame(cv::Mat &frame);                    // 分割フレームを圧縮
};

#endif  /* DISPLAY_SYNCHRONIZER_HPP */

