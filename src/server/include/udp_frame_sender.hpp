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
        udp_t::socket sock;                          // UDPソケット
        std::vector<udp_t::endpoint> endpoint_list;  // 送信エンドポイント
         
        void run() override;                                          // 送信処理を開始
        void sendFrame() override;                                    // フレームを送信
        void onSendFrame(const err_t& err, size_t t_bytes) override;  // フレーム送信時のコールバック
    public:
        UDPFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf, const int port, const int display_num, std::vector<std::string>& ip_list, std::atomic<bool>& send_semaphore);  // コンストラクタ
};

#endif  /* UDP_FRAME_SENDER_HPP */

