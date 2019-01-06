/***************************
*    frame_encoder.cpp     *
*    (フレーム符号化器)    *
***************************/

#ifndef FRAME_ENCODER_HPP
#define FRAME_ENCODER_HPP

#include "mutex_logger.hpp"
#include "ring_buffer.hpp"
#include "sync_utils.hpp"
#include <vector>
#include <atomic>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
extern "C"{
    #include <turbojpeg.h>
}

/* フレーム符号化器 */
class FrameEncoder{
    private:
        const tjhandle handle;                  // JPEGエンコーダ
        cv::VideoCapture video;                 // 再生動画
        const int display_num;                  // 全ディスプレイ数
        cv::Mat bg_frame;                       // リサイズフレームの背景
        int ratio;                              // リサイズ倍率
        cv::Rect roi;                           // リサイズフレームの貼り付け位置
        std::atomic<int>& sampling_type;        // クロマサブサンプリング形式
        std::atomic<int>& quality;              // 量子化品質係数
        std::vector<cv::Rect> regions;          // ディスプレイノードの担当領域
        std::vector<cv::Mat> raw_frames;        // ディスプレイノード毎の生フレーム
        std::vector<jpegbuf_ptr_t>& send_bufs;  // 送信フレームバッファ
        
        void setResizeParams(const int column, const int row,  // リサイズ用パラメータを設定
                             const int width, const int height,
                             const int frame_w, const int frame_h);
        void resize(cv::Mat& video_frame);                        // フレームをリサイズ
        void encode(const int sampling_type, const int quality);  // フレームをJPEGで符号化
    
    public:
        FrameEncoder(const std::string video_src, const int column, const int row, // コンストラクタ
                     const int width, const int height, std::atomic<int>& sampling_type,
                     std::atomic<int>& quality, std::vector<jpegbuf_ptr_t>& send_bufs);
        ~FrameEncoder();  // デストラクタ
        void run();       // フレーム符号化を開始
};

#endif  /* FRAME_ENCODER_HPP */

