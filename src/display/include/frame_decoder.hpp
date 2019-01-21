/**************************
*    frame_decoder.hpp    *
*     (フレーム展開器)    *
**************************/

#ifndef FRAME_DECODER_HPP
#define FRAME_DECODER_HPP

#include "mutex_logger.hpp"
#include "transceive_framebuffer.hpp"
#include "view_framebuffer.hpp"
extern "C"{
    #include <turbojpeg.h>
}

const int JPEG_FAILED = -1;    // JPEG展開失敗時の返り値
const int FRAME_ID_INDEX = 0;  // フレーム番号の開始位置
const int FRAME_ID_LEN = 1;    // フレーム番号の長さ

/* フレーム展開器 */
class FrameDecoder{
    private:
        const tjhandle handle;         // JPEGデコーダ
        const tranbuf_ptr_t recv_buf;  // 受信フレームバッファ
        const viewbuf_ptr_t view_buf;  // 表示フレームバッファ
        
        void decode(unsigned char *jpeg_frame, const unsigned long jpeg_size,  // フレームを展開
                    const int id);
    
    public:
        FrameDecoder(const tranbuf_ptr_t recv_buf, const viewbuf_ptr_t view_buf);  // コンストラクタ
        ~FrameDecoder();  // デストラクタ
        void run();       // フレーム展開を開始
};

#endif  /* FRAME_DECODER_HPP */

