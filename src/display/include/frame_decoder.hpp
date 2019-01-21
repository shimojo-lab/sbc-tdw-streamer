/**************************
*    frame_decoder.hpp    *
*     (フレーム展開器)    *
**************************/

#ifndef FRAME_DECODER_HPP
#define FRAME_DECODER_HPP

#include "mutex_logger.hpp"
#include "json_handler.hpp"
#include "transceive_framebuffer.hpp"
#include "view_framebuffer.hpp"
extern "C"{
    #include <turbojpeg.h>
}

const int JPEG_FAILED = -1;  // JPEG展開失敗時の返り値

/* フレーム展開器 */
class FrameDecoder{
    private:
        const tjhandle handle;         // JPEGデコーダ
        const tranbuf_ptr_t recv_buf;  // 受信フレームバッファ
        const viewbuf_ptr_t view_buf;  // 表示フレームバッファ
        JsonHandler recv_frame;        // 受信フレーム
        
        void decode(unsigned char *jpeg_frame, const unsigned long jpeg_size,  // フレームを展開
                    const int id);
    
    public:
        FrameDecoder(const tranbuf_ptr_t recv_buf, const viewbuf_ptr_t view_buf);  // コンストラクタ
        ~FrameDecoder();  // デストラクタ
        void run();       // フレーム展開を開始
};

#endif  /* FRAME_DECODER_HPP */

