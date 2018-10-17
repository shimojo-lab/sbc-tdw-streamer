/********************************
 *      frame_receiver.hpp     *
 *   (フレーム受信モジュール)  *
 *******************************/

#ifndef FRAME_RECEIVER_HPP
#define FRAME_RECEIVER_HPP

#include "frame_viewer.hpp"
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#define SEPARATOR "\r\n\r\n"  // TCPメッセージのセパレータ

/* 名前空間のエイリアス */
namespace _asio = boost::asio;
namespace _system = boost::system;
namespace _ip = boost::asio::ip;
namespace _ph = boost::asio::placeholders;

/* フレーム受信モジュール */
class FrameReceiver{
    private:
        _asio::io_service &ios;        // BoostのI/Oオブジェクト
        _ip::tcp::socket sock;         // 受信用のTCPソケット
        _ip::tcp::acceptor acc;        // BoostのTCPメッセージ受信器
        _asio::streambuf receive_buf;  // 受信用バッファ
        FrameViewer &viewer;           // フレーム表示モジュール
    public:
        FrameReceiver(_asio::io_service &ios, FrameViewer &viewer, int port);  // コンストラクタ
        ~FrameReceiver();                                  // デストラクタ
        void onConnect(const _system::error_code &error);  // 接続時のコールバック関数
        inline void onReceive(const _system::error_code &error, size_t size);  // 受信時のコールバック関数
};

#endif

