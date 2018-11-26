/**********************************
 *    base_frame_receiver.hpp     *
 *  (フレーム受信器の基底クラス)  *
 *********************************/

#ifndef BASE_FRAME_RECEIVER_HPP
#define BASE_FRAME_RECEIVER_HPP

#include "boost_socket.hpp"
#include "frame_queue.hpp"
#include "print_with_mutex.hpp"

/* フレーム受信器の基底クラス */
class BaseFrameReceiver{
    private:
        virtual void startConnect(const std::string ip, const int port);  // TCP接続待機を開始
        virtual void onConnect(const err_t& err);                          // TCP接続時のコールバック
        virtual void onRecvFrame(const err_t& err, size_t t_bytes);        // フレーム受信時のコールバック
    
    protected:
        ios_t& ios;            // I/Oイベントループ
        const fq_ptr_t queue;  // 分割フレーム用キュー
    
    public:
        BaseFrameReceiver(ios_t& ios, const fq_ptr_t queue);  // コンストラクタ
};

#endif  /* BASE_FRAME_RECEIVER_HPP */

