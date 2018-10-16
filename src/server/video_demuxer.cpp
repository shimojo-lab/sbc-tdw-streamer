/** video_demuxer.cpp **/

#include "video_demuxer.hpp"

// コンストラクタ
VideoDemuxer::VideoDemuxer(const char *filename, int x, int y){
    // パラメータを設定
    this->x = x;
    this->y = y;
    
    // 動画ファイルを読み込み
    if(this->loadVideo(filename) == false){
        exit(-1);
    }
}

// デストラクタ
VideoDemuxer::~VideoDemuxer(){
    delete[] this->rects;
    delete[] this->div_frames;
}

// 動画ファイルを読み込むメソッド
bool VideoDemuxer::loadVideo(const char *filename){
    // 動画ファイルを読み込み
    cv::VideoCapture video(filename);
    if(video.isOpened()){
        this->video = video;
    }else{
        std::cerr << "[Error] File open failed." << std::endl;
        return false;
    }
    // 動画ファイルの情報を取得
    int width = int(video.get(CV_CAP_PROP_FRAME_WIDTH)/this->x);
    int height = int(video.get(CV_CAP_PROP_FRAME_HEIGHT)/this->y);
    this->total_frame_num = video.get(CV_CAP_PROP_FRAME_COUNT);
    this->fps = video.get(CV_CAP_PROP_FPS);
    
    // フレームの分割方法を決定
    int display_num = x * y;
    this->rects = new cv::Rect[display_num];
    this->div_frames = new cv::Mat[display_num];
    for(int j=0; j<this->y; ++j){
        for(int i=0; i<this->x; ++i){
           this->rects[i+this->x*j] = cv::Rect(i*width, j*height, width, height);
        }
    }
    return true;
}

// 次番のフレームを分割するメソッド
void VideoDemuxer::divideNextFrame(){
    // 動画からフレームを取得
    cv::Mat frame;
    this->video >> frame;
    
    // フレームを分割
    for(int j=0; j<this->y; ++j){
       for(int i=0; i<this->x; ++i){
           int idx = i + this->x * j;
           this->div_frames[idx] = cv::Mat(frame, this->rects[idx]);
       }
    }
    return;
}

