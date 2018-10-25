/*******************************
*      frame_streamer.cpp      *
*   (フレーム送信モジュール)   *
*******************************/

#ifndef FRAME_STREAMER_HPP
#define FRAME_STREAMER_HPP

#include "video_demuxer.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>

/* 定数の定義 */
extern const char* const SEPARATOR;  // TCPメッセージのセパレータ

/* 名前空間のエイリアス */
namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _sys = boost::system;
namespace _ph = boost::asio::placeholders;

/* フレーム送信モジュール */
class FrameStreamer{
    private:
        _asio::io_service &ios;               // BoostのI/Oオブジェクト
        _ip::tcp::socket sock;                // 送信用のTCPソケット
        VideoDemuxer &demuxer;                // フレーム分割モジュール
        const char* const ip;                 // 接続先のIPアドレス
        const int id;                         // ディスプレイノードのID
        std::vector<unsigned char> comp_buf;  // 画像圧縮用バッファ
    public:
        FrameStreamer(_asio::io_service &ios, VideoDemuxer &demuxer, const char *ip, int port, int id);  // コンストラクタ
        ~FrameStreamer();                                  // デストラクタ
        void connectByTCP(const char *ip, int port);       // 受信側にTCPで接続
        void onTCPConnect(const _sys::error_code &error);  // TCP接続時のコールバック関数
        void sendFrame(cv::Mat *frame);                    // 分割フレームを送信
        void onSendFrame(const _sys::error_code &error, size_t size);  // フレーム送信時のコールバック関数
};

#endif

