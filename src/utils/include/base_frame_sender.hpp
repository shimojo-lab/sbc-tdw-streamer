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
        std::vector<unsigned char> comp_buf;  // フレーム圧縮用バッファ
        
        virtual void startConnect();                                 // TCP接続待機を開始
        virtual void onConnect(const err_t& err);                    // TCP接続時のコールバック
        virtual void onSendFrame(const err_t& err, size_t t_bytes);  // フレーム送信時のコールバック
    
    protected:
        ios_t& ios;                 // I/Oイベントループ
        const fq_ptr_t queue;       // 分割フレーム用キュー
        
        BaseFrameSender(ios_t& ios, const fq_ptr_t queue);      // コンストラクタ
        const std::string compressFrame(const cv::Mat& frame);  // 分割フレームを圧縮
};

#endif  /* FRAME_SENDER_HPP */

