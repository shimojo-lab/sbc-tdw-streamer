/***************************
*    frame_encoder.cpp     *
*    (フレーム符号化器)    *
***************************/

#ifndef FRAME_ENCODER_HPP
#define FRAME_ENCODER_HPP

#include "ring_buffer.hpp"
#include "print_with_mutex.hpp"
#include <cstdlib>
#include <vector>
#include <tuple>
#include <atomic>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using frame_size_t = std::tuple<int, int>;

const int INIT_CBUF_SIZE = 1024;

/* フレーム符号化器 */
class FrameEncoder{
    private:
        cv::VideoCapture video;           // 再生動画
        msgbuf_ptr_t sbuf;                // 送信フレームバッファ
        std::vector<unsigned char> cbuf;  // フレーム圧縮用バッファ
        std::vector<int> enc_params;      // フレーム圧縮パラメータ
        std::atomic<int>& quality;        // フレーム圧縮品質
        
        std::string compressByJPEG(cv::Mat& frame);  // フレームをJPEG圧縮
        
    public:
        FrameEncoder(const std::string video_src, const msgbuf_ptr_t sbuf, const int cbuf_size, std::atomic<int>& quality);  // コンストラクタ
        frame_size_t getFrameSize();  // フレームサイズを取得
        void run();                   // フレーム符号化を開始
};

#endif  /* FRAME_ENCODER_HPP */

