/*********************************
*     base_frame_sender.cpp      *
*  (フレーム送信器の基底クラス)  *
*********************************/

#include "base_frame_sender.hpp"

/* コンストラクタ */
BaseFrameSender::BaseFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf):
    ios(ios),
    sbuf(sbuf)
{}

/* TCP接続待機を開始 */
void BaseFrameSender::startConnect(){}

/* TCP接続時のコールバック */
void BaseFrameSender::onConnect(const err_t& err){}

/* フレームを送信 */
void BaseFrameSender::sendFrame(){}

/* フレーム送信時のコールバック */
void BaseFrameSender::onSendFrame(const err_t& err, size_t t_bytes){}

