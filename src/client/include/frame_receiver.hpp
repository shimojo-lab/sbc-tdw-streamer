/******************************
 *     frame_receiver.hpp     *
 *    (分割フレーム受信器)    *
 ******************************/

#ifndef FRAME_RECEIVER_HPP
#define FRAME_RECEIVER_HPP

#include "frame_queue.hpp"
#include "print_with_mutex.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/thread/barrier.hpp>

extern const std::string SEPARATOR;  // 受信メッセージのセパレータ
extern const int SEPARATOR_LEN;      // 受信メッセージのセパレータの長さ

namespace _asio = boost::asio;
namespace _sys = boost::system;
namespace _ip = boost::asio::ip;
namespace _ph = boost::asio::placeholders;
using ios_ptr_t = std::shared_ptr<_asio::io_service>;
using tcps_ptr_t = std::shared_ptr<_ip::tcp::socket>;

/* 分割フレーム受信器 */
class FrameReceiver{
    private:
        const ios_ptr_t ios;         // I/Oイベントループ
        const tcps_ptr_t tcp_sock;   // TCPソケット
        _ip::udp::socket udp_sock;   // UDPソケット
        _asio::streambuf tcp_recv_buf;   // 受信メッセージ用バッファ
        boost::array<unsigned char, 1920*1080*3> udp_recv_buf;  // 
        const fq_ptr_t queue;        // 分割フレーム用キュー
        const std::string protocol;  // フレーム受信用プロトコル
        
        void onConnect(const _sys::error_code& err);  // TCP接続時のコールバック
        void onRecvByTCP(const _sys::error_code& err, std::size_t t_bytes);  // TCPでのフレーム受信時のコールバック
        void onRecvByUDP(const _sys::error_code& err, std::size_t t_bytes);  // UDPでのフレーム受信時のコールバック
    public:
        FrameReceiver(const ios_ptr_t ios, const tcps_ptr_t tcp_sock, const fq_ptr_t queue, const std::string ip, const int port, const std::string protocol);  // コンストラクタ
};

#endif  /* FRAME_RECEIVER_HPP */

