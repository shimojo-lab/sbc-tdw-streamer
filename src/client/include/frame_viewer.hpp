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
extern "C"{
    #include <fcntl.h>
    #include <unistd.h>
    #include <linux/fb.h>
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #include <sys/mman.h>
}

const int FB_ALPHA = 255;  // アルファチャンネル値

/* フレーム表示器 */
class FrameViewer{
    private:
        ios_t& ios;                   // I/Oイベントループ
        tcp_t::socket& sock;          // TCPソケット
        _asio::streambuf stream_buf;  // TCP受信用バッファ
        const framebuf_ptr_t vbuf;    // 表示バッファ
        int fb;                       // フレームバッファ
        int fb_len;                   // フレームバッファのサイズ
        char *fb_ptr;                 // フレームバッファの先頭
        MemoryChecker mem_checker;    // メモリ残量確認器
        
        void sendSync();                                    // 同期メッセージを送信
        void displayFrame();                                // フレームを表示
        void onRecvSync(const err_t& err, size_t t_bytes);  // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes);  // 同期メッセージ送信時のコールバック
        
    public:
        FrameViewer(ios_t& ios, tcp_t::socket& sock, const framebuf_ptr_t vbuf, const double threshold, const std::string fb_dev);  // コンストラクタ
};

#endif  /* FRAME_VIEWER_HPP */

