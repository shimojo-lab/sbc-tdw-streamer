/*********************************
*    base_frame_receiver.cpp     *
*  (フレーム受信器の基底クラス)  *
*********************************/

#include "base_frame_receiver.hpp"

/* コンストラクタ */
BaseFrameReceiver::BaseFrameReceiver(_ios& ios, _ios::strand& strand, const fq_ptr_t queue):
    ios(ios),
    strand(strand),
    queue(queue)
{}

/* TCP接続待機を開始 */
void BaseFrameReceiver::startConnect(const std::string ip, const int port){}

/* TCP接続時のコールバック関数 */
void BaseFrameReceiver::onConnect(const _err& err){}

/* TCPでのフレーム受信時のコールバック */
void BaseFrameReceiver::onRecvFrame(const _err& err, size_t t_bytes){}

