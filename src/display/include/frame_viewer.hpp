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
#include "viewer_framebuffer.hpp"
#include <cstring>
extern "C"{
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/mman.h>
    #include <linux/fb.h>
    #include <linux/kd.h>
}

const int DEVICE_OPEN_FAILED = -1;  // デバイスファイルオープン失敗時の返り値
const int BITS_PER_PIXEL = 24;      // 1ピクセルあたりのビット数

/* フレーム表示器 */
class FrameViewer{
    private:
        _asio::io_service& ios;         // I/Oイベントループ
        _ip::tcp::socket& sock;         // TCPソケット
        _asio::streambuf stream_buf;    // ストリームバッファ
        const jpegbuf_ptr_t recv_buf;   // 受信フレームバッファ
        const rawbuf_ptr_t view_buf;    // 表示フレームバッファ
        mutable int fb;                 // フレームバッファ
        mutable int tty;                // デバイス端末
        mutable int fb_size;            // フレームバッファのサイズ
        mutable unsigned char *fb_ptr;  // フレームバッファの先頭
        unsigned char *next_frame;      // 次番フレーム
        
        const bool openFramebuffer(const std::string fb_dev,  // フレームバッファをオープン
                                   const int width, const int height);
        const bool hideCursor(const std::string tty_dev);     // カーソルを非表示化
        void displayFrame();                                  // フレームを表示
        void sendSync();                                      // 同期メッセージを送信
        void onRecvSync(const err_t& err, size_t t_bytes);    // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes);    // 同期メッセージ送信時のコールバック
    
    public:
        FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock,  // コンストラクタ
                    const jpegbuf_ptr_t recv_buf, const rawbuf_ptr_t view_buf);
        ~FrameViewer();  // デストラクタ
};

#endif  /* FRAME_VIEWER_HPP */

