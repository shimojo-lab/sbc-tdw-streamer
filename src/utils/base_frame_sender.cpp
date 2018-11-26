/*********************************
*     base_frame_sender.cpp      *
*  (フレーム送信器の基底クラス)  *
*********************************/

#include "base_frame_sender.hpp"

/* コンストラクタ */
BaseFrameSender::BaseFrameSender(ios_t& ios, const fq_ptr_t queue):
    ios(ios),
    queue(queue)
{}

/* TCP接続待機を開始 */
void BaseFrameSender::startConnect(){}

/* TCP接続時のコールバック */
void BaseFrameSender::onConnect(const err_t& err){}

/* フレーム送信時のコールバック */
void BaseFrameSender::onSendFrame(const err_t& err, size_t t_bytes){}

/* フレームを圧縮 */
const std::string BaseFrameSender::compressFrame(const cv::Mat& frame){
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
    cv::imencode(".jpg", frame, this->comp_buf, params);
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += SEPARATOR;
    return bytes_buf;
}

