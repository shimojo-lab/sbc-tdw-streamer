/*******************************
*       frame_sender.hpp       *
*     (分割フレーム送信器)     *
********************************/

#ifndef FRAME_SENDER_HPP
#define FRAME_SENDER_HPP

#include "frame_queue.hpp"
#include "print_with_mutex.hpp"
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <opencv2/imgcodecs.hpp>

namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _sys = boost::system;
namespace _ph = boost::asio::placeholders;
using ios_ptr_t = std::unique_ptr<_asio::io_service>;

extern const std::string SEPARATOR;  // 受信メッセージのセパレータ
extern const int SEPARATOR_LEN;      // 受信メッセージのセパレータの長さ

/* 分割フレーム送信器 */
class FrameSender{
    private:
        const ios_ptr_t& ios;                 // I/Oイベントループ
        _ip::tcp::socket tcp_sock;            // TCPソケット
        _ip::udp::socket udp_sock;            // UDPソケット
        _ip::tcp::acceptor acc;               // TCPメッセージ受信器
        _asio::streambuf recv_buf;            // メッセージ受信用バッファ
        _ip::udp::endpoint udp_endpoint;      // UDPでの送信先
        const fq_ptr_t& queue;                // 分割フレーム用キュー
        const int udp_port;                   // 送信先のUDP用ポート
        const std::string protocol;           // フレーム送信用プロトコル
        std::vector<unsigned char> comp_buf;  // フレーム圧縮用バッファ
        
        void onConnect(const _sys::error_code& err);                              // TCP接続時のコールバック
        void onSendFrameByTCP(const _sys::error_code& err, std::size_t t_bytes);  // TCPでのフレーム送信時のコールバック
        void onSendFrameByUDP(const _sys::error_code& err, std::size_t t_bytes);  // UDPでのフレーム送信時のコールバック
        void onRecvSync(const _sys::error_code& err, std::size_t t_bytes);  // 同期メッセージ受信時のコールバック
        void onSendSync(const _sys::error_code& err, std::size_t t_bytes);  // 同期メッセージ送信時のコールバック
        inline std::string compressFrame(cv::Mat& frame);                         // 分割フレームを圧縮
    public:
        FrameSender(const ios_ptr_t& ios, const fq_ptr_t& queue, const int port, const std::string protocol);  // コンストラクタ
};

/* 分割フレームを圧縮 */
inline std::string FrameSender::compressFrame(cv::Mat &frame){
    // フレームを圧縮
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
    cv::imencode(".jpg", frame, this->comp_buf, params);
    
    // 送信用バイト列を作成
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += SEPARATOR;
    return bytes_buf;
}

#endif  /* FRAME_SENDER_HPP */

