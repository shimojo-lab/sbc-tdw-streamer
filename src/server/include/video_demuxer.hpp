/** video_demuxer.hpp **/

#ifndef VIDEO_DEMUXER_HPP
#define VIDEO_DEMUXER_HPP

#include <iostream>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

/* 動画フレーム分割器 */
class VideoDemuxer{
    private:
        cv::VideoCapture video;  // 再生する動画
        cv::Rect *rects;         // フレームの分割領域
        cv::Mat *div_frames;     // 分割フレーム
        int x;                   // ディスプレイの横の枚数
        int y;                   // ディスプレイの縦の枚数
        int frame_num = 0;       // 現在のフレーム番号
        int total_frame_num;     // 総フレーム数
        double fps;              // 動画のフレームレート
    public:
        VideoDemuxer(const char *filename, int x, int y);  // コンストラクタ
        ~VideoDemuxer();                                   // デストラクタ
        bool loadVideo(const char *filename);              // 動画ファイルを読み込み
        void divideNextFrame();                            // 次番のフレームを分割
        cv::Mat *getDividedFrame(int x, int y);            // 分割フレームを取得
};

// 分割フレームを取得するメソッド
inline cv::Mat *VideoDemuxer::getDividedFrame(int x, int y){
    return &(this->div_frames[x*this->y*y]);
}

#endif

