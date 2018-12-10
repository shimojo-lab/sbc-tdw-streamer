/*********************************
*    base_frame_receiver.cpp     *
*  (フレーム受信器の基底クラス)  *
*********************************/

#include "base_frame_receiver.hpp"

/* コンストラクタ */
BaseFrameReceiver::BaseFrameReceiver(ios_t& ios, const msgbuf_ptr_t rbuf):
    ios(ios),
    rbuf(rbuf)
{}

/* 受信処理を開始 */
void BaseFrameReceiver::run(const std::string ip, const int port){}

/* フレーム受信時のコールバック */
void BaseFrameReceiver::onRecvFrame(const err_t& err, size_t t_bytes){}

