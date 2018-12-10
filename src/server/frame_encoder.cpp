/**************************
*    frame_encoder.cpp    *
*    (フレーム符号化器)   *
**************************/

#include "frame_encoder.hpp"

/* コンストラクタ */
FrameEncoder::FrameEncoder(const std::string video_src, const msgbuf_ptr_t sbuf, const int cbuf_size, std::atomic<int>& quality):
    sbuf(sbuf),
    cbuf(cbuf_size),
    enc_params(2),
    quality(quality)
{
    // 再生動画を読込み
    print_info("Opened playback video. (" + video_src + ")");
    this->video = cv::VideoCapture(video_src.c_str());
    if(!this->video.isOpened()){
        print_err("Failed to open video", video_src);
        std::exit(EXIT_FAILURE);
    }
}

/* フレームサイズを取得 */
frame_size_t FrameEncoder::getFrameSize(){
    const int width = this->video.get(CV_CAP_PROP_FRAME_WIDTH);
    const int height = this->video.get(CV_CAP_PROP_FRAME_HEIGHT);
    return std::forward_as_tuple(width, height);
}

/* フレームをJPEG圧縮 */
std::string FrameEncoder::compressByJPEG(cv::Mat& frame){
    this->enc_params[0] = cv::IMWRITE_JPEG_QUALITY;
    this->enc_params[1] = this->quality;
    cv::imencode(".jpg", frame, this->cbuf, this->enc_params);
    std::string bytes_frame(this->cbuf.begin(), this->cbuf.end());
    return bytes_frame;
}

/* フレーム符号化を開始 */
void FrameEncoder::run(){
    cv::Mat frame;
    while(true){
        try{
            this->video >> frame;
        }catch(...){
            print_err("Failed to get video frame", "Video ended");
            return;
        }
        sbuf->push(this->compressByJPEG(frame));
    }
}

