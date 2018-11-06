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
        _udp::socket udp_sock;      // UDPソケット
        
        void startConnect() override;                                    // TCP接続待機を開始
        void onConnect(const _err& err) override;                    // TCP接続時のコールバック
        void onSendFrame(const _err& err, size_t t_bytes) override;  // フレーム送信時のコールバック
    
    public:
        UDPFrameSender(const ios_ptr_t ios, const fq_ptr_t queue, const int port, const bar_ptr_t barrier);  // コンストラクタ

};

#endif  /* UDP_FRAME_SENDER_HPP */

