/*********************************
 *       frame_receiver.cpp     *
 *    (フレーム送信モジュール)  *
 ********************************/

#ifndef FRAME_STREAMER_HPP
#define FRAME_STREAMER_HPP

#include "video_demuxer.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#define SEPARATOR "\r\n\r\n"  // TCPメッセージのセパレータ

/* 名前空間のエイリアス */
namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _system = boost::system;
namespace _ph = boost::asio::placeholders;

/* フレーム送信モジュール */
class FrameStreamer{
    private:
        _asio::io_service &ios;               // BoostのI/Oオブジェクト
        _ip::tcp::socket sock;                // 送信用のTCPソケット
        const char *ip;                       // 接続先のIPアドレス
        VideoDemuxer &demuxer;                // フレーム分割モジュール
        std::vector<unsigned char> comp_buf;  // 画像圧縮用バッファ
    public:
        FrameStreamer(_asio::io_service &ios, VideoDemuxer &demuxer, const char *ip, int port);  // コンストラクタ
        ~FrameStreamer();                                  // デストラクタ
        void connectByTCP(const char *ip, int port);       // 受信側にTCPで接続
        void onConnect(const _system::error_code &error);  // 接続時のコールバック関数
        bool sendFrame(cv::Mat &frame);                    // 分割フレームを送信
};

#endif

