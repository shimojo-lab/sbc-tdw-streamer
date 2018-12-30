/*************************
*    frame_sender.hpp    *
*    (フレーム送信器)    *
*************************/

#ifndef FRAME_SENDER_HPP
#define FRAME_SENDER_HPP

#include "mutex_logger.hpp"
#include "ring_buffer.hpp"
#include "socket_utils.hpp"
#include "sync_utils.hpp"
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

/* フレーム送信器 */
class FrameSender{
    private:
        _asio::io_service& ios;                 // I/Oイベントループ
        sock_ptr_t sock;                        // TCPソケット
        _ip::tcp::acceptor acc;                 // TCPアクセプタ
        std::vector<sock_ptr_t> socks;          // 接続済TCPソケット
        long frame_num = 0;                     // フレーム番号
        const int display_num;                  // 全ディスプレイ数
        std::atomic<int> send_count;            // 送信完了数
        std::atomic<bool>& send_semaphore;      // 送信制御用セマフォ
        std::vector<jpegbuf_ptr_t>& send_bufs;  // 送信フレームバッファ
        
        void run();                                          // 送信処理を開始
        void sendFrame();                                    // フレームを送信
        void onConnect(const err_t& err);                    // ディスプレイノード接続時のコールバック
        void onSendFrame(const err_t& err, size_t t_bytes);  // フレーム送信時のコールバック
    
    public:
        FrameSender(_asio::io_service& ios, const int port,  // コンストラクタ
                    const int display_num, std::atomic<bool>& send_semaphore,
                    std::vector<jpegbuf_ptr_t>& send_bufs);
};

#endif  /* FRAME_SENDER_HPP */

