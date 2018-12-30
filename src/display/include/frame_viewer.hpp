/*************************
*    frame_viewer.hpp    *
*    (フレーム表示器)    *
*************************/

#ifndef FRAME_VIEWER_HPP
#define FRAME_VIEWER_HPP

#include "mutex_logger.hpp"
#include "ring_buffer.hpp"
#include "socket_utils.hpp"
#include "sync_utils.hpp"
#include "memory_checker.hpp"
extern "C"{
    #include <fcntl.h>
    #include <unistd.h>
    #include <linux/fb.h>
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #include <sys/mman.h>
}

const int DEF_PIXEL_VALUE = 0;  // デフォルトの輝度値

/* フレーム表示器 */
class FrameViewer{
    private:
        _asio::io_service& ios;       // I/Oイベントループ
        _ip::tcp::socket& sock;       // TCPソケット
        _asio::streambuf stream_buf;  // ストリームバッファ
        const framebuf_ptr_t vbuf;    // 表示フレームバッファ
        int fb;                       // フレームバッファ
        int fb_len;                   // フレームバッファのサイズ
        char *fb_ptr;                 // フレームバッファの先頭
        MemoryChecker mem_checker;    // メモリ残量確認器
        
        const bool openFrameBuffer(const std::string fb_dev);  // フレームバッファをオープン
        void clearFrame();                                     // フレームバッファをクリア
        void displayFrame();                                   // フレームを表示
        void sendSync();                                       // 同期メッセージを送信
        void onRecvSync(const err_t& err, size_t t_bytes);     // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes);     // 同期メッセージ送信時のコールバック
        
    public:
        FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock,  // コンストラクタ
                    const framebuf_ptr_t vbuf, const double threshold, const std::string fb_dev);
        ~FrameViewer();  // デストラクタ
};

#endif  /* FRAME_VIEWER_HPP */

