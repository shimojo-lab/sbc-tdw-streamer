/*******************************
*       frame_sender.hpp       *
*     (分割フレーム送信器)     *
********************************/

#ifndef FRAME_SENDER_HPP
#define FRAME_SENDER_HPP

#include "frame_queue.hpp"
#include <vector>
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

/* 変数型の定義 */
using smt_ios_t = std::shared_ptr<_asio::io_service>;

/* 分割フレーム送信器 */
class FrameSender{
    private:
        smt_ios_t ios;                        // BoostのI/Oオブジェクト
        _ip::tcp::socket sock;                // TCPソケット
        smt_fq_t queue;                       // 分割フレーム用キュー
        const char* ip;                       // 送信先のIP
        std::vector<unsigned char> comp_buf;  // フレーム圧縮用バッファ
    public:
        FrameSender(smt_ios_t ios, smt_fq_t queue);        // コンストラクタ
        ~FrameSender();                                    // デストラクタ
        void start(const char *ip, int port);              // 送信処理を開始
        void onTCPConnect(const _sys::error_code &error);  // TCP接続時のコールバック
        std::string compressFrame(cv::Mat &frame);         // 分割フレームを圧縮
        void onSend(const _sys::error_code &error, std::size_t bytes_transferred);  // 分割フレーム送信時のコールバック
};

#endif  /* FRAME_SENDER_HPP */

