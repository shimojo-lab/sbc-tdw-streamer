/**********************************
 *    base_frame_receiver.hpp     *
 *  (フレーム受信器の基底クラス)  *
 *********************************/

#ifndef BASE_FRAME_RECEIVER_HPP
#define BASE_FRAME_RECEIVER_HPP

#include "socket_utils.hpp"
#include "ring_buffer.hpp"
#include "print_with_mutex.hpp"

/* フレーム受信器の基底クラス */
class BaseFrameReceiver{
    private:
        virtual void startConnect(const std::string ip, const int port);  // TCP接続待機を開始
        virtual void onConnect(const err_t& err);                         // TCP接続時のコールバック
        virtual void onRecvFrame(const err_t& err, size_t t_bytes);       // フレーム受信時のコールバック
    
    protected:
        ios_t& ios;               // I/Oイベントループ
        const msgbuf_ptr_t rbuf;  // 受信バッファ
    
    public:
        BaseFrameReceiver(ios_t& ios, const msgbuf_ptr_t rbuf);  // コンストラクタ
};

#endif  /* BASE_FRAME_RECEIVER_HPP */

