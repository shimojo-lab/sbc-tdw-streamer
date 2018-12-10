/****************************
*   tcp_frame_sender.hpp    *
*   (TCP版フレーム送信器)   *
****************************/

#ifndef TCP_FRAME_SENDER_HPP
#define TCP_FRAME_SENDER_HPP

#include "base_frame_sender.hpp"
#include "print_with_mutex.hpp"
#include <vector>
#include <atomic>

/* TCP版フレーム送信器 */
class TCPFrameSender : public BaseFrameSender{
    private:
        tcps_ptr_t sock;                    // TCPソケット
        tcp_t::acceptor acc;                // TCPメッセージ受信器
        std::vector<tcps_ptr_t> sock_list;  // 接続済TCPソケット
        const msgbuf_ptr_t sbuf;            // 送信バッファ
        const int display_num;              // 全ディスプレイ数
        std::atomic<int> send_count;        // 送信完了数
         
        void run() override;                                          // 送信処理を開始
        void onConnect(const err_t& err);                             // 接続時のコールバック
        void sendFrame() override;                                    // フレームを送信
        void onSendFrame(const err_t& err, size_t t_bytes) override;  // フレーム送信時のコールバック
    public:
        TCPFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf, const int port, const int display_num);  // コンストラクタ
};

#endif  /* TCP_FRAME_SENDER_HPP */

