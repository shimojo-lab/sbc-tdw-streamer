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

/* フレーム展開器 */
class FrameDecoder{
    private:
        const tjhandle handle;         // JPEGデコーダ
        const tranbuf_ptr_t recv_buf;  // 受信フレームバッファ
        const rawbuf_ptr_t view_buf;   // 表示フレームバッファ
        
        void decode();  // フレームを展開
    
    public:
        FrameDecoder(const tranbuf_ptr_t recv_buf, const rawbuf_ptr_t view_buf);  // コンストラクタ
        ~FrameDecoder();  // デストラクタ
        void run();       // フレーム展開を開始
};

#endif  /* FRAME_DECODER_HPP */

