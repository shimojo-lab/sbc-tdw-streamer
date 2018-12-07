/****************************
*   udp_frame_sender.hpp    *
*   (UDP版フレーム送信器)   *
****************************/

#ifndef UDP_FRAME_SENDER_HPP
#define UDP_FRAME_SENDER_HPP

#include "base_frame_sender.hpp"
#include "print_with_mutex.hpp"
#include <vector>
#include <atomic>

/* UDP版フレーム送信器 */
class UDPFrameSender : public BaseFrameSender{
    private:
        std::vector<udps_ptr_t> sock_list;  // UDPソケット
        const int display_num;              // 全ディスプレイ数
        std::atomic<int> send_count;        // 送信完了数
         
        void startConnect() override;                                 // 接続待機を開始
        void onConnect(const err_t& err) override;                    // 接続時のコールバック
        void sendFrame() override;                                    // フレームを送信
        void onSendFrame(const err_t& err, size_t t_bytes) override;  // フレーム送信時のコールバック
    public:
        TCPFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf, const int port, const int display_num);  // コンストラクタ
};

#endif  /* UDP_FRAME_SENDER_HPP */

