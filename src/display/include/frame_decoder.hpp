/**************************
*    frame_decoder.hpp    *
*     (フレーム展開器)    *
**************************/

#ifndef FRAME_DECODER_HPP
#define FRAME_DECODER_HPP

#include "mutex_logger.hpp"
#include "ring_buffer.hpp"
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

/* フレーム展開器 */
class FrameDecoder{
    private:
        const msgbuf_ptr_t rbuf;    // 受信バッファ
        const framebuf_ptr_t vbuf;  // 表示バッファ
     
    public:
        FrameDecoder(const msgbuf_ptr_t rbuf, const framebuf_ptr_t vbuf);  // コンストラクタ
        void run();  // フレーム展開を開始
};

#endif  /* FRAME_DECODER_HPP */

