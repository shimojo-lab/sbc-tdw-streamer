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

/* TCP接続待機を開始 */
void BaseFrameReceiver::startConnect(const std::string ip, const int port){}

/* TCP接続時のコールバック関数 */
void BaseFrameReceiver::onConnect(const err_t& err){}

/* フレーム受信時のコールバック */
void BaseFrameReceiver::onRecvFrame(const err_t& err, size_t t_bytes){}

