/****************************
*   udp_frame_sender.hpp    *
*   (UDP版フレーム送信器)   *
****************************/

#ifndef UDP_FRAME_SENDER_HPP
#define UDP_FRAME_SENDER_HPP

#include "base_frame_sender.hpp"

/* UDP版フレーム送信器 */
class UDPFrameSender : public BaseFrameSender{
    private:
        tcp_t::socket sock;   // UDPソケット
        
        void startConnect() override;                                 // UDP接続待機を開始
        void onConnect(const err_t& err) override;                    // TCP接続時のコールバック
        void onSendFrame(const err_t& err, size_t t_bytes) override;  // フレーム送信時のコールバック
    public:
        TCPFrameSender(ios_t& ios, const fq_ptr_t queue, const int port);  // コンストラクタ
};

#endif  /* UDP_FRAME_SENDER_HPP */

