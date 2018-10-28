/*****************************
 *     video_splitter.cpp    *
 *      (フレーム分割器)     *
 *****************************/

#ifndef VIDEO_SPLITTER_HPP
#define VIDEO_SPLITTER_HPP

#include "frame_queue.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

/* フレーム分割器 */
class VideoSplitter{
    private:
        cv::VideoCapture video;             // 再生する動画
        const int row;                      // ディスプレイの横の枚数
        const int column;                   // ディスプレイの縦の枚数
        int total_frame_num;                // 総フレーム数
        double fps;                         // 動画のフレームレート
        std::vector<cv::Rect> region_list;  // フレームの分割領域
        std::vector<smt_fq_t> queue_list;   // 分割フレーム用キュー
        cv::Mat frame;                      // 分割前のフレーム
        void setVideoParams();              // 分割時のパラメータを設定
    public:
        VideoSplitter(const char *video_src, int row, int column);  // コンストラクタ
        ~VideoSplitter();                                           // デストラクタ
        const smt_fq_t getFrameQueuePtr(const int id);              // 分割フレーム用キューのポインタを取得
        void start();                                               // フレームの分割を開始
};

#endif  /* VIDEO_SPLITTER_HPP */

