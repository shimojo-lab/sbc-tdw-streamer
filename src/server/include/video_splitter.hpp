/***************************
*    video_splitter.cpp    *
*     (フレーム分割器)     *
***************************/

#ifndef VIDEO_SPLITTER_HPP
#define VIDEO_SPLITTER_HPP

#include "frame_queue.hpp"
#include "print_with_mutex.hpp"
#include <vector>
#include <tuple>
#include <cstdlib>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

using video_params_t = std::tuple<int, int>;

/* フレーム分割器 */
class VideoSplitter{
    private:
        cv::VideoCapture video;             // 再生する動画
        const int row;                      // ディスプレイの横の枚数
        const int column;                   // ディスプレイの縦の枚数
        int frame_num;                      // 総フレーム数
        std::vector<cv::Rect> region_list;  // フレームの分割領域
        std::vector<fq_ptr_t> queue_list;   // 分割フレーム用キュー
        cv::Mat frame;                      // 分割前のフレーム
        
        void setSplitParams();  // 分割用のパラメータを設定
    public:
        VideoSplitter(const char* const video_src, const int row, const int column);  // コンストラクタ
        video_params_t getVideoParams();                // 動画の情報を取得
        const fq_ptr_t getFrameQueuePtr(const int id);  // 分割フレーム用キューのポインタを取得
        void start();                                   // フレームの分割を開始
};

#endif  /* VIDEO_SPLITTER_HPP */

