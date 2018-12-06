/*************************
*    frame_viewer.hpp    *
*    (フレーム表示器)    *
*************************/

#ifndef FRAME_VIEWER_HPP
#define FRAME_VIEWER_HPP

#include "frame_queue.hpp"
#include "print_with_mutex.hpp"
#include "socket_utils.hpp"
#include "sdl2_wrapper.hpp"
#include <opencv2/highgui.hpp>

/* 分割フレーム表示部 */
class FrameViewer{
    private:
        ios_t& ios;                 // I/Oイベントループ
        tcp_t::socket& sock;        // TCPソケット
        const fq_ptr_t queue;       // 分割フレームキュー
        const SDL2Wrapper sdl2;     // SDL2のラッパー
        const int width;            // フレームの横の長さ
        const int height;           // フレームの縦の長さ
        const int frame_num;        // 総フレーム数
        _asio::streambuf recv_buf;  // 受信メッセージ用バッファ
        cv::Mat frame;              // 表示フレーム
        
        void onRecvSync(const err_t& err, size_t t_bytes);  // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes);  // 同期メッセージ送信時のコールバック
    
    public:
        FrameViewer(ios_t& ios, tcp_t::socket& sock, const fq_ptr_t queue, const int res_x, const int res_y, const int width, const int height, const int frame_num, const int frame_rate);  // コンストラクタ
};

#endif  /* FRAME_VIEWER_HPP */

