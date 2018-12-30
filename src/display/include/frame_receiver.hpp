/***************************
*    frame_receiver.hpp    *
*     (フレーム受信器)     *
***************************/

#ifndef FRAME_RECEIVER_HPP
#define FRAME_RECEIVER_HPP

#include "mutex_logger.hpp"
#include "socket_utils.hpp"
#include "ring_buffer.hpp"

/* フレーム受信器 */
class FrameReceiver{
    private:
        _asio::io_service ios;        // I/Oイベントループ
        tcp_t::socket sock;           // TCPソケット
        _asio::streambuf stream_buf;  // ストリームバッファ
        const msgbuf_ptr_t rbuf;      // 受信フレームバッファ
        
        void run(const std::string ip, const int port);      // 受信処理を開始
        void onConnect(const err_t& err);                    // 接続時のコールバック
        void onRecvFrame(const err_t& err, size_t t_bytes);  // フレーム受信時のコールバック
    
    public:
        FrameReceiver(_asio::io_service& ios, const msgbuf_ptr_t rbuf, const std::string ip, const int port);  // コンストラクタ
};

#endif  /* FRAME_RECEIVER_HPP */

