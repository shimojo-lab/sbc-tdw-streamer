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
#include <boost/thread/barrier.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

/* 分割フレーム表示部 */
class FrameViewer{
    private:
        _asio::io_service::strand& strand;  // I/O排他制御
        const tcps_ptr_t tcp_sock;          // TCPソケット
        const fq_ptr_t queue;               // 分割フレームキュー
        const SDL2Wrapper sdl2;             // SDL2のラッパー
        const int width;                    // フレームの横の長さ
        const int height;                   // フレームの縦の長さ
        const int frame_num;                // 総フレーム数
        _asio::streambuf recv_buf;          // 受信メッセージ用バッファ
        boost::barrier& barrier;            // 同期用バリア
        
        void sendSync();  // 同期メッセージを送信
        void onRecvSync(const _sys::error_code& err, std::size_t t_bytes);  // 同期メッセージ受信時のコールバック
        void onSendSync(const _sys::error_code&err, std::size_t t_bytes);   // 同期メッセージ送信時のコールバック
        const cv::Mat decompressFrame();                // フレームを展開
        void displayFrame(const cv::Mat& frame);  // フレームを表示
    public:
        FrameViewer(_asio::io_service::strand& strand, const tcps_ptr_t tcp_sock, const fq_ptr_t queue, const int res_x, const int res_y, const int width, const int height, const int framerate, const int frame_num, boost::barrier& barrier);  // コンストラクタ
        void run();    // フレーム表示を開始
};

#endif  /* FRAME_VIEWER_HPP */

