/**************************
*    frame_decoder.cpp    *
*     (フレーム展開器)    *
**************************/

#include "frame_decoder.hpp"
#include <chrono>

/* コンストラクタ */
FrameDecoder::FrameDecoder(const msgbuf_ptr_t rbuf, const framebuf_ptr_t vbuf):
    rbuf(rbuf),
    vbuf(vbuf)
{}

/* フレーム展開を開始 */
void FrameDecoder::run(){
    cv::Mat frame;
    while(true){
const auto start = std::chrono::system_clock::now();
        const std::string msg = this->rbuf->pop();
        std::vector<unsigned char> comp_buf(msg.c_str(), msg.c_str()+msg.length());
        this->vbuf->push(cv::imdecode(comp_buf, CV_LOAD_IMAGE_UNCHANGED));
const auto end = std::chrono::system_clock::now();
double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
print_debug(elapsed);
    }
}

