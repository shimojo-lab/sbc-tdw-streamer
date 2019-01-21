/***************************
*    frame_encoder.cpp     *
*    (フレーム符号化器)    *
***************************/

#ifndef FRAME_ENCODER_HPP
#define FRAME_ENCODER_HPP

#include "mutex_logger.hpp"
#include "transceive_framebuffer.hpp"
#include <vector>
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
extern "C"{
    #include <turbojpeg.h>
}

const int COLOR_CHANNEL_NUM = 3;  // 色のチャネル数
const int JPEG_FAILED = -1;       // JPEG圧縮失敗時の返り値

/* フレーム圧縮器 */
class FrameEncoder{
    private:
        const tjhandle handle;                  // JPEGエンコーダ
        cv::VideoCapture video;                 // 再生動画
        const int display_num;                  // 全ディスプレイ数
        double ratio;                           // リサイズ倍率
        int interpolation_type;                 // リサイズ方式
        cv::Mat resized_frame;                  // リサイズ後のフレーム
        cv::Rect roi;                           // リサイズフレームの貼り付け位置
        std::atomic<int>& sampling_type;        // クロマサブサンプル比
        std::atomic<int>& quality;              // JPEG品質係数
        std::vector<cv::Rect> regions;          // ディスプレイノードの各領域
        std::vector<cv::Mat> raw_frames;        // ディスプレイノードの各フレーム
        std::vector<tranbuf_ptr_t>& send_bufs;  // 送信フレームバッファ
        
        void setResizeParams(const int column, const int row,        // リサイズ用パラメータを設定
                             const int bezel_w, const int bezel_h,
                             const int width, const int height,
                             const int frame_w, const int frame_h);
        void resize(cv::Mat& video_frame);                           // フレームをリサイズ
        void encode(const int sampling_type, const int quality);     // フレームをJPEG圧縮
    
    public:
        FrameEncoder(const std::string video_src, const int column,  // コンストラクタ
                     const int row, const int bezel_w, const int bezel_h, const int width,
                     const int height, std::atomic<int>& sampling_type, std::atomic<int>& quality,
                     std::vector<tranbuf_ptr_t>& send_bufs);
        ~FrameEncoder();  // デストラクタ
        void run();       // フレーム圧縮を開始
};

#endif  /* FRAME_ENCODER_HPP */

