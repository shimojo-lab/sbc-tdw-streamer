/*********************************
*      base_frame_sender.hpp     *
*  (フレーム送信器の基底クラス)  *
*********************************/

#ifndef BASE_FRAME_SENDER_HPP
#define BASE_FRAME_SENDER_HPP

#include "boost_socket.hpp"
#include "frame_queue.hpp"
#include "print_with_mutex.hpp"
#include <vector>
#include <opencv2/imgcodecs.hpp>

/* フレーム送信器の基底クラス */
class BaseFrameSender{
    private:
        _ios& ios;                            // I/Oイベントループ
        std::vector<unsigned char> comp_buf;  // フレーム圧縮用バッファ
        
        virtual void startConnect();                                // TCP接続待機を開始
        virtual void onConnect(const _err& err);                    // TCP接続時のコールバック
        virtual void onSendFrame(const _err& err, size_t t_bytes);  // フレーム送信時のコールバック
    
    protected:
        _ios::strand strand;        // I/O排他制御
        _tcp::socket sock;          // TCPソケット
        _tcp::acceptor acc;         // TCP受信器
        _asio::streambuf recv_buf;  // TCP受信用バッファ
        const fq_ptr_t queue;       // 分割フレーム用キュー
        const bar_ptr_t barrier;    // 同期用バリア
        
        BaseFrameSender(_ios& ios, const fq_ptr_t queue, const int port, const bar_ptr_t barrier);  // コンストラクタ
        void onRecvSync(const _err& err, size_t t_bytes);       // 同期メッセージ受信時のコールバック
        void onSendSync(const _err& err, size_t t_bytes);       // 同期メッセージ送信時のコールバック
        const std::string compressFrame(const cv::Mat& frame);  // 分割フレームを圧縮
};

#endif  /* FRAME_SENDER_HPP */

