/*********************************
*     base_frame_sender.cpp      *
*  (フレーム送信器の基底クラス)  *
*********************************/

#include "base_frame_sender.hpp"

/* コンストラクタ */
BaseFrameSender::BaseFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf, const int display_num, std::atomic<bool>& send_semaphore):
    ios(ios),
    sbuf(sbuf),
    display_num(display_num),
    send_semaphore(send_semaphore)
{}

/* 送信処理を開始 */
void BaseFrameSender::run(){}

/* フレームを送信 */
void BaseFrameSender::sendFrame(){}

/* フレーム送信時のコールバック */
void BaseFrameSender::onSendFrame(const err_t& err, size_t t_bytes){}

