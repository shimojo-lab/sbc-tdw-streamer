/**************************
*    frame_decoder.cpp    *
*     (フレーム展開器)    *
**************************/

#include "frame_decoder.hpp"
#include <chrono>

/* コンストラクタ */
FrameDecoder::FrameDecoder(const jpegbuf_ptr_t recv_buf, const rawbuf_ptr_t view_buf):
    handle(tjInitDecompress()),
    recv_buf(recv_buf),
    view_buf(view_buf)
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
    std::string jpeg_frame_bytes = this->recv_buf->pop();
    const int iter = jpeg_frame_bytes.length() - VIEWBUF_ID_LEN;
    const int id = std::stoi(jpeg_frame_bytes.substr(iter));
    jpeg_frame_bytes.erase(iter);
    const unsigned long jpeg_size = (unsigned long)jpeg_frame_bytes.length();
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
    tjDecompress2(this->handle,
                  jpeg_frame.data(),
                  jpeg_size,
                  this->view_buf->getFramebuffer(id),
                  frame_w,
                  frame_w*FRAME_COLORS,
                  frame_h,
                  TJPF_RGB,
                  TJFLAG_FASTDCT|TJFLAG_FASTUPSAMPLE
    );
    
    // 表示フレームバッファに反映
    this->view_buf->activateFramebuffer();
}

/* フレーム展開を開始 */
void FrameDecoder::run(){
    while(true){
//const auto start = std::chrono::system_clock::now();
        try{
            this->decode();
        }catch(...){
            print_warn("Failed to get new video frame", "JPEG decode error");
        }
//const auto end = std::chrono::system_clock::now();
//double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
//print_debug(elapsed);
    }
}

