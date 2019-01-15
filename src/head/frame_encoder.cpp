/**************************
*    frame_encoder.cpp    *
*    (フレーム符号化器)   *
**************************/

#include "frame_encoder.hpp"

/* コンストラクタ */
FrameEncoder::FrameEncoder(const std::string video_src, const int column, const int row, const int bezel_w, const int bezel_h,
                           const int width, const int height, std::atomic<int>& sampling_type,
                           std::atomic<int>& quality, std::vector<tranbuf_ptr_t>& send_bufs):
    handle(tjInitCompress()),
    display_num(column*row),
    sampling_type(sampling_type),
    quality(quality),
    send_bufs(send_bufs)
{
    // JPEGエンコーダを初期化
    if(this->handle == NULL){
        std::string err_msg(tjGetErrorStr());
        _ml::caution("Failed to init JPEG encoder", err_msg);
        std::exit(EXIT_FAILURE);
    }
    
    // 再生動画を読込み
    this->video = cv::VideoCapture(video_src.c_str());
    if(!this->video.isOpened()){
        _ml::caution("Failed to open video", video_src);
        std::exit(EXIT_FAILURE);
    }
    
    // リサイズ用パラメータを設定
    cv::Mat video_frame;
    this->video >> video_frame;
    this->setResizeParams(column, row, bezel_w, bezel_h, width, height, video_frame.cols, video_frame.rows);
}

/* デストラクタ */
FrameEncoder::~FrameEncoder(){
    // JPEGエンコーダを破棄
    tjDestroy(this->handle);
}

/* リサイズ用パラメータを設定 */
void FrameEncoder::setResizeParams(const int column, const int row, const int bezel_w, const int bezel_h, const int width, const int height,
                                   const int frame_w, const int frame_h){
    // リサイズフレームの背景を設定
    const int bg_w = column * width + (column - 1) * bezel_w;
    const int bg_h = row * height + (row - 1) * bezel_h;
    this->bg_frame = cv::Mat::zeros(bg_h, bg_w, CV_8UC3);
    
    // リサイズ倍率を設定
    const int x_ratio = (int)((double)bg_w / (double)frame_w);
    const int y_ratio = (int)((double)bg_h / (double)frame_h);
    this->ratio = x_ratio < y_ratio ? x_ratio : y_ratio;
    
    // リサイズフレームのパディングを設定
    const int resize_w = frame_w * this->ratio;
    const int resize_h = frame_h * this->ratio;
    const int paste_x = (int)((double)(bg_w-resize_w) / 2.0);
    const int paste_y = (int)((double)(bg_h-resize_h) / 2.0);
    this->roi = cv::Rect(paste_x, paste_y, resize_w, resize_h);
    
    // ディスプレイノードの担当領域を設定
    this->regions = std::vector<cv::Rect>(this->display_num);
    this->raw_frames = std::vector<cv::Mat>(this->display_num);
    for(int j=0; j<row; ++j){
        for(int i=0; i<column; ++i){
            this->regions[i+column*j] = cv::Rect((width+bezel_w)*i, (height+bezel_h)*j, width, height);
        }
    }
}

/* フレームをリサイズ */
void FrameEncoder::resize(cv::Mat& video_frame){
    // アスペクト比を維持して拡大
    cv::resize(video_frame, video_frame, cv::Size(), this->ratio, this->ratio, CV_INTER_LINEAR);
    
    // 背景と重ねてパディング
    cv::Mat resized_frame;
    this->bg_frame.copyTo(resized_frame);
    cv::Mat paste_area(resized_frame, this->roi);
    video_frame.copyTo(paste_area);
    
    // ディスプレイノードの担当領域毎に分割
    for(int i=0; i<this->display_num; ++i){
        this->raw_frames[i] = cv::Mat(resized_frame, this->regions[i]);
    }
}

/* フレームをJPEGで符号化 */
void FrameEncoder::encode(const int sampling_type, const int quality){
    for(int i=0; i<this->display_num; ++i){
        unsigned char *jpeg_frame = NULL;
        unsigned long jpeg_size = 0;
        const int tj_stat = tjCompress2(handle,
                                        raw_frames[i].data,
                                        raw_frames[i].cols,
                                        raw_frames[i].cols*COLOR_CHANNEL_NUM,
                                        raw_frames[i].rows,
                                        TJPF_RGB,
                                        &jpeg_frame,
                                        &jpeg_size,
                                        sampling_type,
                                        quality,
                                        TJFLAG_FASTDCT
        );
        if(tj_stat == JPEG_FAILED){
            const std::string err_msg(tjGetErrorStr());
            _ml::warn("JPEG encode failed", err_msg);
        }else{
            const std::string jpeg_frame_bytes(jpeg_frame, jpeg_frame+jpeg_size);
            this->send_bufs[i]->push(jpeg_frame_bytes);
        }
    }
}

/* フレーム圧縮を開始 */
void FrameEncoder::run(){
    while(true){
        cv::Mat video_frame;
        this->video >> video_frame;
        try{
            this->resize(video_frame);
        }catch(...){
            _ml::caution("Could not get video frame", "JPEG encoder stopped");
            break;
        }
        this->encode(this->sampling_type.load(std::memory_order_acquire), this->quality.load(std::memory_order_acquire));
    }
}

