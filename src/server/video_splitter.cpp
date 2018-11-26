/*****************************
*     video_splitter.cpp     *
*      (フレーム分割器)      *
*****************************/

#include "video_splitter.hpp"

/* コンストラクタ */
VideoSplitter::VideoSplitter(const std::string video_src, const int row, const int column):
    row(row),
    column(column)
{
    // 再生する動画を読込み
    print_info("Opened playback video. (" + video_src + ")");
    this->video = cv::VideoCapture(video_src.c_str());
    if(!this->video.isOpened()){
        print_err("Failed to open video", video_src);
        std::exit(EXIT_FAILURE);
    }else{
        // 分割用のパラメータを設定
        this->setSplitParams();
    }
}

/* 分割用のパラメータを設定 */
void VideoSplitter::setSplitParams(){
    // 総フレーム数を取得
    this->frame_num = this->video.get(CV_CAP_PROP_FRAME_COUNT);
    
    // フレームレートを取得
    this->frame_rate = this->video.get(CV_CAP_PROP_FPS);
    
    // 分割領域とキューを設定
    const int width = int(this->video.get(CV_CAP_PROP_FRAME_WIDTH)/this->row);
    const int height = int(this->video.get(CV_CAP_PROP_FRAME_HEIGHT)/this->column);
    const int display_num = this->row * this->column;
    this->region_list = std::vector<cv::Rect>(display_num);
    this->queue_list = std::vector<fq_ptr_t>(display_num);
    for(int y=0; y<this->column; ++y){
        for(int x=0; x<this->row; ++x){
           int id = x + this->row * y;
           this->region_list[id] = cv::Rect(x*width, y*height, width, height);
           this->queue_list[id] = std::make_shared<FrameQueue>(32);
        }
    }
}

/* 再生動画の情報を取得 */
video_params_t VideoSplitter::getVideoParams(){
    const int frame_num = this->frame_num;
    const double frame_rate = this->frame_rate;
    return std::forward_as_tuple(frame_num, frame_rate);
}

/* 分割フレーム用キューのポインタを取得 */
const fq_ptr_t VideoSplitter::getFrameQueuePtr(const int id){
    return this->queue_list[id];
}

/* フレームの分割を開始 */
void VideoSplitter::run(){
    cv::Mat frame;
    for(int i=0; i<this->frame_num; ++i){
        // 次番のフレームを分割
        this->video >> frame;
        for(int y=0; y<this->column; ++y){
            for(int x=0; x<this->row; ++x){
                int id = x + this->row * y;
                this->queue_list[id]->enqueue(cv::Mat(frame, this->region_list[id]));
            }
        }
    }
}

