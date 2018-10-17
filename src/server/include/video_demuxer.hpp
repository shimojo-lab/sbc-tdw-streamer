/********************************
 *      video_demuxer.cpp       *
 *   (フレーム分割モジュール)   *
 ********************************/

#ifndef VIDEO_DEMUXER_HPP
#define VIDEO_DEMUXER_HPP

#include <iostream>
#include <cstdlib>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

/* 動画フレーム分割器 */
class VideoDemuxer{
    private:
        cv::VideoCapture video;         // 再生する動画
        cv::Rect *rects;                // フレームの分割領域
        cv::Mat *div_frames;            // 分割フレーム
        int row;                        // ディスプレイの横の枚数
        int column;                     // ディスプレイの縦の枚数
        int frame_num = 0;              // 現在のフレーム番号
        int total_frame_num;            // 総フレーム数
        double fps;                     // 動画のフレームレート
        inline void divideNextFrame();  // 次番のフレームを分割
    public:
        VideoDemuxer(const char *filename, int row, int column);  // コンストラクタ
        ~VideoDemuxer();                                          // デストラクタ
        inline cv::Mat *getDividedFrame(int x, int y);            // 分割フレームを取得
};

#endif

