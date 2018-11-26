/****************************
*   tcp_frame_sender.hpp    *
*   (TCP版フレーム送信器)   *
****************************/

#ifndef TCP_FRAME_SENDER_HPP
#define TCP_FRAME_SENDER_HPP

#include "base_frame_sender.hpp"

/* TCP版フレーム送信器 */
class TCPFrameSender : public BaseFrameSender{
    private:
        tcp_t::socket sock;   // TCPソケット
        tcp_t::acceptor acc;  // TCPメッセージ受信器
        
        void startConnect() override;                                 // TCP接続待機を開始
        void onConnect(const err_t& err) override;                    // TCP接続時のコールバック
        void onSendFrame(const err_t& err, size_t t_bytes) override;  // フレーム送信時のコールバック
    public:
        TCPFrameSender(ios_t& ios, const fq_ptr_t queue, const int port);  // コンストラクタ
};

#endif  /* TCP_FRAME_SENDER_HPP */

