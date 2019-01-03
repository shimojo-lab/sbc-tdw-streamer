/**************************
*    frame_decoder.cpp    *
*     (フレーム展開器)    *
**************************/

#include "frame_decoder.hpp"
#include <chrono>

/* コンストラクタ */
FrameDecoder::FrameDecoder(const jpegbuf_ptr_t rbuf, const matbuf_ptr_t vbuf):
    handle(tjInitDecompress()),
    rbuf(rbuf),
    vbuf(vbuf)
{
    // JPEGデコータを起動
    if(this->handle == NULL){
        std::string err_msg(tjGetErrorStr());
        print_err("Failed to init JPEG decoder", err_msg);
        std::exit(EXIT_FAILURE);
    }
}

/* デストラクタ */
FrameDecoder::~FrameDecoder(){
    // JPEGデコーダを破棄
    tjDestroy(this->handle);
}

/* フレームを複号 */
void FrameDecoder::decode(){
    // フレームを取り出し
    const std::string jpeg_frame_bytes = this->rbuf->pop();
    unsigned long jpeg_size = (unsigned long)jpeg_frame_bytes.length();
    std::vector<unsigned char> jpeg_frame(jpeg_frame_bytes.c_str(), jpeg_frame_bytes.c_str()+jpeg_size);
    
    // JPEGのヘッダを読み込み
    int frame_w, frame_h, sampling_type;
    tjDecompressHeader2(this->handle,
                        jpeg_frame.data(),
                        jpeg_size,
                        &frame_w,
                        &frame_h,
                        &sampling_type
    );
    
    // フレームを複号
    unsigned char *raw_frame = new unsigned char[frame_w*frame_h*FRAME_COLORS];
    tjDecompress2(this->handle,
                  jpeg_frame.data(),
                  jpeg_size,
                  raw_frame,
                  frame_w,
                  0,
                  frame_h,
                  TJPF_BGR,
                  0
    );
    cv::Mat mat_raw_frame(frame_w, frame_h, CV_8UC3, raw_frame);
    this->vbuf->push(mat_raw_frame);
}

/* フレーム展開を開始 */
void FrameDecoder::run(){
    while(true){
//const auto start = std::chrono::system_clock::now();
        try{
        }catch(...){
            print_warn("Failed to get new video frame", "JPEG decode error");
        }
            this->decode();
//const auto end = std::chrono::system_clock::now();
//double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
//print_debug(elapsed);
    }
}

