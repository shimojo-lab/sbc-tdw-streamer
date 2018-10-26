/******************************
 *     frame_receiver.hpp     *
 *    (分割フレーム受信器)    *
 ******************************/

#ifndef FRAME_RECEIVER_HPP
#define FRAME_RECEIVER_HPP

#include "frame_queue.hpp"
#include <iostream>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

/* 定数の定義 */
extern const char* const SEPARATOR;  // 受信メッセージのセパレータ
extern const int SEPARATOR_LEN;      // 受信メッセージのセパレータの長さ

/* 名前空間のエイリアス */
namespace _asio = boost::asio;
namespace _sys = boost::system;
namespace _ip = boost::asio::ip;
namespace _ph = boost::asio::placeholders;

/* 分割フレーム受信器 */
class FrameReceiver{
    private:
        _asio::io_service &ios;        // BoostのI/Oオブジェクト
        _ip::tcp::socket sock;         // TCPソケット
        _ip::tcp::acceptor acc;        // TCPメッセージ受信器
        _asio::streambuf receive_buf;  // 受信メッセージ用バッファ
        smt_FrameQueue_t queue;        // 分割フレームキュー
    public:
        FrameReceiver(_asio::io_service &ios, smt_FrameQueue_t queue, int port);  // コンストラクタ
        ~FrameReceiver();                                                         // デストラクタ
        void onTCPConnect(const _sys::error_code &error);                         // TCP接続時のコールバック
        inline void onReceive(const _sys::error_code &error, size_t size);        // 受信時のコールバック
};

#endif  /* FRAME_RECEIVER_HPP */

