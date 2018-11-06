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
        void startConnect() override;                                    // TCP接続待機を開始
        void onConnect(const _err& err) override;                    // TCP接続時のコールバック
        void onSendFrame(const _err& err, size_t t_bytes) override;  // フレーム送信時のコールバック
    public:
        TCPFrameSender(_ios& ios, const fq_ptr_t queue, const int port, const bar_ptr_t barrier);  // コンストラクタ
};

#endif  /* TCP_FRAME_SENDER_HPP */

