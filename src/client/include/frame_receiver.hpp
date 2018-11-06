/******************************
 *     frame_receiver.hpp     *
 *    (分割フレーム受信器)    *
 ******************************/

#ifndef FRAME_RECEIVER_HPP
#define FRAME_RECEIVER_HPP

#include "frame_queue.hpp"
#include "print_with_mutex.hpp"
#include "boost_socket.hpp"
#include <boost/array.hpp>

/* 分割フレーム受信器 */
class FrameReceiver{
    private:
        const ios_ptr_t ios;                // I/Oイベントループ
        _asio::io_service::strand& strand;  // I/O排他制御
        const tcps_ptr_t tcp_sock;          // TCPソケット
        _ip::udp::socket udp_sock;          // UDPソケット
        _asio::streambuf tcp_recv_buf;      // 受信メッセージ用バッファ
        boost::array<unsigned char, 1920*1080*3> udp_recv_buf;  // 
        const fq_ptr_t queue;               // 分割フレーム用キュー
        const std::string protocol;         // フレーム受信用プロトコル
        
        void onConnect(const _sys::error_code& err);  // TCP接続時のコールバック
        void onRecvByTCP(const _sys::error_code& err, std::size_t t_bytes);  // TCPでのフレーム受信時のコールバック
        void onRecvByUDP(const _sys::error_code& err, std::size_t t_bytes);  // UDPでのフレーム受信時のコールバック
    public:
        FrameReceiver(const ios_ptr_t ios, _asio::io_service::strand& strand, const tcps_ptr_t tcp_sock, const fq_ptr_t queue, const std::string ip, const int port, const std::string protocol);  // コンストラクタ
};

#endif  /* FRAME_RECEIVER_HPP */

