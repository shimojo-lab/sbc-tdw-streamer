/*************************
*    frame_viewer.hpp    *
*    (フレーム表示器)    *
*************************/

#ifndef FRAME_VIEWER_HPP
#define FRAME_VIEWER_HPP

#include "socket_utils.hpp"
#include "sync_utils.hpp"
#include "ring_buffer.hpp"
#include "print_with_mutex.hpp"
#include "memory_checker.hpp"
#include <opencv2/highgui.hpp>

/* フレーム表示器 */
class FrameViewer{
    private:
        ios_t& ios;                 // I/Oイベントループ
        tcp_t::socket& sock;        // TCPソケット
        const framebuf_ptr_t vbuf;  // 表示バッファ
        MemoryChecker mem_checker;  // メモリ残量確認器
        _asio::streambuf recv_buf;  // 受信メッセージ用バッファ
        
        void sendSync();                                    // 同期メッセージを送信
        void displayFrame();                                // フレームを表示
        void onRecvSync(const err_t& err, size_t t_bytes);  // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes);  // 同期メッセージ送信時のコールバック
        
    public:
        FrameViewer(ios_t& ios, tcp_t::socket& sock, const framebuf_ptr_t vbuf, const double threshold);  // コンストラクタ
};

#endif  /* FRAME_VIEWER_HPP */

