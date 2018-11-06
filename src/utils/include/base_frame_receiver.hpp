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
        _ios& ios;  // I/Oイベントループ
        
        virtual void startConnect(const std::string ip, const int port);  // TCP接続待機を開始
        virtual void onConnect(const _err& err);                          // TCP接続時のコールバック
        virtual void onRecvFrame(const _err& err, size_t t_bytes);        // フレーム受信時のコールバック
    
    protected:
        _ios::strand& strand;  // I/O排他制御
        const fq_ptr_t queue;  // 分割フレーム用キュー
    
    public:
        BaseFrameReceiver(_ios& ios, _ios::strand& strand, const fq_ptr_t queue);  // コンストラクタ
};

#endif  /* BASE_FRAME_RECEIVER_HPP */

