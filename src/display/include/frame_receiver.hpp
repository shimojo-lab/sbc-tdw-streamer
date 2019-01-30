/***************************
*    frame_receiver.hpp    *
*     (フレーム受信器)     *
***************************/

#ifndef FRAME_RECEIVER_HPP
#define FRAME_RECEIVER_HPP

#include "mutex_logger.hpp"
#include "socket_utils.hpp"
#include "transceive_framebuffer.hpp"

/* フレーム受信器 */
class FrameReceiver{
    private:
        _asio::io_service& ios;        // I/Oイベントループ
        _ip::tcp::socket sock;         // TCPソケット
        _asio::streambuf stream_buf;   // ストリームバッファ
        const tranbuf_ptr_t recv_buf;  // 受信フレームバッファ
        
    public:
        FrameReceiver(_asio::io_service& ios, const tranbuf_ptr_t recv_buf);  // コンストラクタ
        void run(const std::string& ip_addr, const int port);                 // 受信処理を開始
};

#endif  /* FRAME_RECEIVER_HPP */

