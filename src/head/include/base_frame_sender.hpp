/*********************************
*      base_frame_sender.hpp     *
*  (フレーム送信器の基底クラス)  *
*********************************/

#ifndef BASE_FRAME_SENDER_HPP
#define BASE_FRAME_SENDER_HPP

#include "socket_utils.hpp"
#include "sync_utils.hpp"
#include "ring_buffer.hpp"
#include "print_with_mutex.hpp"
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

/* フレーム送信器の基底クラス */
class BaseFrameSender{
    private:
        virtual void run();                                          // 送信処理を開始
        virtual void sendFrame();                                    // フレームを送信
        virtual void onSendFrame(const err_t& err, size_t t_bytes);  // フレーム送信時のコールバック
        
    protected:
        ios_t& ios;                         // I/Oイベントループ
        msgbuf_ptr_t sbuf;                  // 送信フレームバッファ
        long sequence_num = 0;              // フレームのシーケンス番号
        const int display_num;              // 全ディスプレイ数
        std::atomic<int> send_count;        // 送信完了数
        std::atomic<bool>& send_semaphore;  // 送信制御用セマフォ
        
        BaseFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf, const int display_num, std::atomic<bool>& send_semaphore);  // コンストラクタ
};

#endif  /* FRAME_SENDER_HPP */

