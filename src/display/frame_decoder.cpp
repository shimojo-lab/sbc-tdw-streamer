/**************************
*    frame_decoder.cpp    *
*     (フレーム展開器)    *
**************************/

#include "frame_decoder.hpp"

/* コンストラクタ */
FrameDecoder::FrameDecoder(const tranbuf_ptr_t recv_buf, const viewbuf_ptr_t view_buf):
    handle(tjInitDecompress()),
    recv_buf(recv_buf),
    view_buf(view_buf)
{
    // JPEGデコータを起動
    if(this->handle == NULL){
        const std::string err_msg(tjGetErrorStr());
        _ml::caution("Failed to init JPEG decoder", err_msg);
        std::exit(EXIT_FAILURE);
    }
}

/* デストラクタ */
FrameDecoder::~FrameDecoder(){
    tjDestroy(this->handle);
}

/* フレームを展開 */
void FrameDecoder::decode(unsigned char *jpeg_frame, const unsigned long jpeg_size, const int id){
    // JPEGのヘッダを読み込み
    int frame_w, frame_h, sampling_type;
    const int tj_stat1 = tjDecompressHeader2(this->handle,
                                             jpeg_frame,
                                             jpeg_size,
                                             &frame_w,
                                             &frame_h,
                                             &sampling_type
    );
    if(tj_stat1 == JPEG_FAILED){
        const std::string err_msg(tjGetErrorStr());
        _ml::warn("Could not get new video frame", err_msg);
        return;
    }
    
    // フレームを複号
    const int tj_stat2 = tjDecompress2(this->handle,
                                       jpeg_frame,
                                       jpeg_size,
                                       this->view_buf->getDrawArea(id),
                                       frame_w,
                                       frame_w*COLOR_CHANNEL_NUM,
                                       frame_h,
                                       TJPF_RGB,
                                       TJFLAG_FASTDCT|TJFLAG_FASTUPSAMPLE
    );
    if(tj_stat2 == JPEG_FAILED){
        const std::string err_msg(tjGetErrorStr());
        _ml::warn("Could not get new video frame", err_msg);
        return;
    }
}

/* フレーム展開を開始 */
void FrameDecoder::run(){
    while(true){
        // 展開時間を計測 (デバッグ用)
        #ifdef DEBUG
        const auto start = std::chrono::system_clock::now();
        #endif
        
        // フレームを取り出し
        std::string jpeg_str = this->recv_buf->pop();
        const int id = std::stoi(jpeg_str.substr(FRAME_ID_INDEX, FRAME_ID_LEN));
        jpeg_str.erase(FRAME_ID_INDEX, FRAME_ID_LEN);
        
        // フレームを展開
        const unsigned long jpeg_size = (unsigned long)jpeg_str.length();
        std::vector<unsigned char> jpeg_frame(jpeg_str.c_str(), jpeg_str.c_str()+jpeg_size);
        this->decode(jpeg_frame.data(), jpeg_size, id);
        this->view_buf->activateFrame(id);
        
        #ifdef DEBUG
        const auto end = std::chrono::system_clock::now();
        const double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        _ml::debug(std::to_string(elapsed) + "fps");
        #endif
    }
}

