/*************************
*    frame_sender.hpp    *
*    (フレーム送信器)    *
*************************/

#ifndef FRAME_SENDER_HPP
#define FRAME_SENDER_HPP

#include "mutex_logger.hpp"
#include "socket_utils.hpp"
#include "json_handler.hpp"
#include "transceive_framebuffer.hpp"
#include <vector>
#include <atomic>

/* フレーム送信器 */
class FrameSender{
    private:
        _asio::io_service& ios;                 // I/Oイベントループ
        sock_ptr_t sock;                        // TCPソケット
        _ip::tcp::acceptor acc;                 // TCPアクセプタ
        std::vector<sock_ptr_t> socks;          // 接続済TCPソケット
        const int display_num;                  // 全ディスプレイ数
        int fb_id = 0;                          // 表示フレームバッファのインデックス
        const int viewbuf_num;                  // 表示フレームバッファの領域数
        std::atomic<int> send_count;            // 送信完了数
        std::vector<JsonHandler> send_msgs;     // 送信メッセージ
        std::vector<tranbuf_ptr_t>& send_bufs;  // 送信フレームバッファ
        
        void run();                                          // 送信処理を開始
        void sendFrame();                                    // フレームを送信
        void onConnect(const err_t& err);                    // ディスプレイノード接続時のコールバック
        void onSendFrame(const err_t& err, size_t t_bytes);  // フレーム送信時のコールバック
    
    public:
        FrameSender(_asio::io_service& ios, const int port,  // コンストラクタ
                    const int display_num, std::vector<tranbuf_ptr_t>& send_bufs,
                    const int viewbuf_num);
};

#endif  /* FRAME_SENDER_HPP */

