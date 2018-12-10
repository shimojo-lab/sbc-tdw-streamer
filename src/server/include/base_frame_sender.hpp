/*********************************
*      base_frame_sender.hpp     *
*  (フレーム送信器の基底クラス)  *
*********************************/

#ifndef BASE_FRAME_SENDER_HPP
#define BASE_FRAME_SENDER_HPP

#include "socket_utils.hpp"
#include "ring_buffer.hpp"

/* フレーム送信器の基底クラス */
class BaseFrameSender{
    private:        
        virtual void run();                                          // 送信処理を開始
        virtual void sendFrame();                                    // フレームを送信
        virtual void onSendFrame(const err_t& err, size_t t_bytes);  // フレーム送信時のコールバック
    
    protected:
        ios_t& ios;            // I/Oイベントループ
        msgbuf_ptr_t sbuf;     // 送信バッファ
        int sequence_num = 0;  // フレームのシーケンス番号
        
        BaseFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf);  // コンストラクタ
};

#endif  /* FRAME_SENDER_HPP */

