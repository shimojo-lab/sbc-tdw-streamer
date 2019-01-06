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

/* フレーム表示器 */
class FrameViewer{
    private:
        _asio::io_service& ios;        // I/Oイベントループ
        _ip::tcp::socket& sock;        // TCPソケット
        _asio::streambuf stream_buf;   // ストリームバッファ
        const jpegbuf_ptr_t recv_buf;  // 受信フレームバッファ
        const rawbuf_ptr_t view_buf;   // 表示フレームバッファ
        
        void sendSync();                                       // 同期メッセージを送信
        void onRecvSync(const err_t& err, size_t t_bytes);     // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes);     // 同期メッセージ送信時のコールバック
    
    public:
        FrameViewer(_asio::io_service& ios, _ip::tcp::socket& sock,  // コンストラクタ
                    const jpegbuf_ptr_t recv_buf, const rawbuf_ptr_t view_buf);
};

#endif  /* FRAME_VIEWER_HPP */

