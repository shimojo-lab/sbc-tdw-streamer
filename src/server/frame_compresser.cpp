/*****************************
*    frame_compresser.cpp    *
*      (フレーム圧縮器)      *
*****************************/

#include "frame_compresser.hpp"

/* コンストラクタ */
FrameCompresser::FrameCompresser(const std::string video_src, const msgbuf_ptr_t sbuf, const int cbuf_size, std::atomic<int>& quality):
    sbuf(sbuf),
    cbuf(cbuf_size),
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

/* フレームをJPEG圧縮 */
std::string FrameCompresser::compressByJPEG(cv::Mat& frame){
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, this->quality};
    cv::imencode(".jpg", frame, this->cbuf, params);
    std::string bytes_frame(this->cbuf.begin(), this->cbuf.end());
    return bytes_frame;
}

/* フレーム圧縮を開始 */
void FrameCompresser::run(){
    cv::Mat frame;
    while(true){
        this->video >> frame;
        sbuf->push(this->compressByJPEG(frame));
    }
}

