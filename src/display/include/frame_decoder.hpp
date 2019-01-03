/**************************
*    frame_decoder.hpp    *
*     (フレーム展開器)    *
**************************/

#ifndef FRAME_DECODER_HPP
#define FRAME_DECODER_HPP

#include "mutex_logger.hpp"
#include "ring_buffer.hpp"
#include <vector>
extern "C"{
    #include <turbojpeg.h>
}

/* フレーム展開器 */
class FrameDecoder{
    private:
        const tjhandle handle;      // JPEGデコーダ
        const jpegbuf_ptr_t rbuf;   // 受信フレームバッファ
        const ucharbuf_ptr_t vbuf;  // 表示フレームバッファ
         
        void decode();  // フレームを展開
    
    public:
        FrameDecoder(const jpegbuf_ptr_t rbuf, const ucharbuf_ptr_t vbuf);  // コンストラクタ
        ~FrameDecoder();  // デストラクタ
        void run();       // フレーム展開を開始
};

#endif  /* FRAME_DECODER_HPP */

