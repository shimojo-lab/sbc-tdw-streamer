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
        
        void run(const std::string& ip_addr, const int port);  // 受信処理を開始
        void onConnect(const err_t& err);                      // 接続時のコールバック
        void onRecvFrame(const err_t& err, size_t t_bytes);    // フレーム受信時のコールバック
    
    public:
        FrameReceiver(_asio::io_service& ios, const std::string ip_addr,  // コンストラクタ
                      const int stream_port, const tranbuf_ptr_t recv_buf);
};

#endif  /* FRAME_RECEIVER_HPP */

