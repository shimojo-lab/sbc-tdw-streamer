#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>

/* 動画フレームの分割を行うクラス */
class VideoDemuxer{
    private:
        cv::VideoCapture video;  // 再生する動画
        cv::Rect *rects;         // 各フレームの分割領域
        int width;               // フレームの横の長さ
        int height;              // フレームの縦の長さ
        int frame_num;           // 総フレーム数
        int display_num;         // ディスプレイ数
        double fps;              // 動画のフレームレート
    public:
        VideoDemuxer(const char* filename, int x, int y);  // コンストラクタ
        cv::Mat getNextFrame(int id);                      // 次番のフレームを取得
};

// コンストラクタ
VideoDemuxer::VideoDemuxer(const char* filename, int x, int y){
    // 動画ファイルを読み込み
    cv::VideoCapture video(filename);
    if(video.isOpened()){
        this->video = video;
    }else{
        std::cerr << "Error: Could not open file." << std::endl;
        exit(-1);
    }
    
    // フレームサイズを取得
    this->width = video.get(CV_CAP_PROP_FRAME_WIDTH);
    this->height = video.get(CV_CAP_PROP_FRAME_HEIGHT);
    this->frame_num = video.get(CV_CAP_PROP_FRAME_COUNT);
    this->fps = video.get(CV_CAP_PROP_FPS);
    
    // フレームの分割方法を決定
    this->display_num = x * y;
    this->rects = new cv::Rect[this->display_num];
    int div_x = int(this->width/x);
    int div_y = int(this->height/y);
    for(int j=0; j<y; j++){
        for(int i=0; i<x; i++){
            this->rects[i+x*j] = cv::Rect(i*div_x, j*div_y, div_x-1, div_y-1);
            std::cout << this->rects[i+x*j];
        }
    }
}

// 次番のフレームを取得するメソッド
cv::Mat VideoDemuxer::getNextFrame(int id){
    cv::Mat frame;
    this->video >> frame;
    cv::Mat trimmed(frame, this->rects[id]);
    return trimmed;
}

/* Main */
int main(int argc, char* argv[]){
    VideoDemuxer demuxer("test2.mp4", 2, 2);
    
    for(;;){
        cv::Mat frame0 = demuxer.getNextFrame(0);
        cv::Mat frame1 = demuxer.getNextFrame(1);
        cv::Mat frame2 = demuxer.getNextFrame(2);
        cv::Mat frame3 = demuxer.getNextFrame(3);
        cv::imshow("0", frame0);
        cv::imshow("1", frame1);
        cv::imshow("2", frame2);
        cv::imshow("3", frame3);
        cv::waitKey(1);
    }
    return 0;
}

