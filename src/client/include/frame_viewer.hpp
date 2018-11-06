/*************************
*    frame_viewer.hpp    *
*    (フレーム表示器)    *
*************************/

#ifndef FRAME_VIEWER_HPP
#define FRAME_VIEWER_HPP

#include "frame_queue.hpp"
#include "print_with_mutex.hpp"
#include "boost_socket.hpp"
#include "sdl2_wrapper.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

/* 分割フレーム表示部 */
class FrameViewer{
    private:
        _ios& ios;                  // I/Oイベントループ
        _ios::strand& strand;       // I/O排他制御
        const tcps_ptr_t sock;      // TCPソケット
        const fq_ptr_t queue;       // 分割フレームキュー
        const SDL2Wrapper sdl2;     // SDL2のラッパー
        const int width;            // フレームの横の長さ
        const int height;           // フレームの縦の長さ
        const int frame_num;        // 総フレーム数
        _asio::streambuf recv_buf;  // 受信メッセージ用バッファ
        cv::Mat frame;              // 表示フレーム
        
        void sendSync();                                   // 同期メッセージを送信
        void onSendSync(const _err& err, size_t t_bytes);  // 同期メッセージ送信時のコールバック
        void onRecvSync(const _err& err, size_t t_bytes);  // 同期メッセージ受信時のコールバック
    
    public:
        FrameViewer(_ios& ios, _ios::strand& strand, const tcps_ptr_t sock, const fq_ptr_t queue, const int res_x, const int res_y, const int width, const int height, const int framerate, const int frame_num);  // コンストラクタ
};

#endif  /* FRAME_VIEWER_HPP */

