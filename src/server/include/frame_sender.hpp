/*******************************
*       frame_sender.hpp       *
*     (分割フレーム送信器)     *
********************************/

#ifndef FRAME_SENDER_HPP
#define FRAME_SENDER_HPP

#include "frame_queue.hpp"
#include "print_with_mutex.hpp"
#include "boost_socket.hpp"
#include <vector>
#include <boost/thread/barrier.hpp>
#include <opencv2/imgcodecs.hpp>

using bar_ptr_t = std::shared_ptr<boost::barrier>;

/* 分割フレーム送信器 */
class FrameSender{
    private:
        const ios_ptr_t ios;                  // I/Oイベントループ
        _asio::io_service::strand& strand;    // I/O排他制御
        _ip::tcp::socket tcp_sock;            // TCPソケット
        _ip::udp::socket udp_sock;            // UDPソケット
        _ip::tcp::acceptor acc;               // TCPメッセージ受信器
        _asio::streambuf recv_buf;            // メッセージ受信用バッファ
        _ip::udp::endpoint udp_endpoint;      // UDPでの送信先
        const fq_ptr_t queue;                 // 分割フレーム用キュー
        const int udp_port;                   // 送信先のUDP用ポート
        const std::string protocol;           // フレーム送信用プロトコル
        std::vector<unsigned char> comp_buf;  // フレーム圧縮用バッファ
        bar_ptr_t barrier;                    // 同期用バリア
        
        void onConnect(const _sys::error_code& err);                              // TCP接続時のコールバック
        void onSendFrameByTCP(const _sys::error_code& err, std::size_t t_bytes);  // TCPでのフレーム送信時のコールバック
        void onSendFrameByUDP(const _sys::error_code& err, std::size_t t_bytes);  // UDPでのフレーム送信時のコールバック
        void onRecvSync(const _sys::error_code& err, std::size_t t_bytes);  // 同期メッセージ受信時のコールバック
        void onSendSync(const _sys::error_code& err, std::size_t t_bytes);  // 同期メッセージ送信時のコールバック
        std::string compressFrame(const cv::Mat& frame);                    // 分割フレームを圧縮
    public:
        FrameSender(const ios_ptr_t ios, _asio::io_service::strand& strand, const fq_ptr_t queue, const int port, const std::string protocol, const bar_ptr_t barrier);  // コンストラクタ

};

#endif  /* FRAME_SENDER_HPP */

