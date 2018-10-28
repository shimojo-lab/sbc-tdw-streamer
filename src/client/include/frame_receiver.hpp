/******************************
 *     frame_receiver.hpp     *
 *    (分割フレーム受信器)    *
 ******************************/

#ifndef FRAME_RECEIVER_HPP
#define FRAME_RECEIVER_HPP

#include "frame_queue.hpp"
#include "console_mutex.hpp"
#include <cstring>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

extern const char* const SEPARATOR;  // 受信メッセージのセパレータ
extern const int SEPARATOR_LEN;      // 受信メッセージのセパレータの長さ

namespace _asio = boost::asio;
namespace _sys = boost::system;
namespace _ip = boost::asio::ip;
namespace _ph = boost::asio::placeholders;

using smt_ios_t = std::shared_ptr<_asio::io_service>;

/* 分割フレーム受信器 */
class FrameReceiver{
    private:
        const smt_ios_t ios;           // BoostのI/Oオブジェクト
        _ip::tcp::socket sock;         // TCPソケット
        _ip::tcp::acceptor acc;        // TCPメッセージ受信器
        _asio::streambuf receive_buf;  // 受信メッセージ用バッファ
        const smt_fq_t queue;          // 分割フレーム用キュー
    public:
        FrameReceiver(const smt_ios_t ios, const smt_fq_t queue, const int port);  // コンストラクタ
        ~FrameReceiver();                                                             // デストラクタ
        void onTCPConnect(const _sys::error_code &error);                             // TCP接続時のコールバック
        inline void onReceive(const _sys::error_code &error, size_t received_bytes);  // 受信時のコールバック
};

#endif  /* FRAME_RECEIVER_HPP */

