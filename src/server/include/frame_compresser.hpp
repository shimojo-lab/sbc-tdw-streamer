/*****************************
*    frame_compresser.cpp    *
*      (フレーム圧縮器)      *
*****************************/

#ifndef FRAME_COMPRESSER_HPP
#define FRAME_COMPRESSER_HPP

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

/* フレーム分割器 */
class FrameCompresser{
    private:
        cv::VideoCapture video;           // 再生動画
        msgbuf_ptr_t sbuf;                // 送信バッファ
        std::vector<unsigned char> cbuf;  // 圧縮バッファ
        std::atomic<int>& quality;        // 圧縮品質
        
        std::string compressByJPEG(cv::Mat& frame);  // フレームをJPEG圧縮
        
    public:
        FrameCompresser(const std::string video_src, const msgbuf_ptr_t sbuf, const int cbuf_size, std::atomic<int>& quality);  // コンストラクタ
        frame_size_t getFrameSize();  // フレームサイズを取得
        void run();                   // フレーム圧縮を開始
};

#endif  /* FRAME_COMPRESSER_HPP */

