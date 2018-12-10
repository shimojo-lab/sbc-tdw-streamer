/******************************
*   tcp_frame_receiver.hpp    *
*    (TCP版フレーム受信器)    *
******************************/

#ifndef TCP_FRAME_RECEIVER_HPP
#define TCP_FRAME_RECEIVER_HPP

#include "base_frame_receiver.hpp"

/* TCP版フレーム受信器 */
class TCPFrameReceiver : public BaseFrameReceiver{
    private:
        tcp_t::socket sock;         // TCPソケット
        _asio::streambuf recv_buf;  // TCP受信用バッファ
        
        void run(const std::string ip, const int port) override;      // 受信処理を開始
        void onConnect(const err_t& err);                             // 接続時のコールバック
        void onRecvFrame(const err_t& err, size_t t_bytes) override;  // フレーム受信時のコールバック
    
    public:
        TCPFrameReceiver(ios_t& ios, const msgbuf_ptr_t rbuf, const std::string ip, const int port);  // コンストラクタ
};

#endif  /* TCP_FRAME_RECEIVER_HPP */

