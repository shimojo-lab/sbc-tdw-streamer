/**************************
*    frame_decoder.hpp    *
*     (フレーム展開器)    *
**************************/

#ifndef FRAME_DECODER_HPP
#define FRAME_DECODER_HPP

#include "print_with_mutex.hpp"
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
        const cv::Rect roi;         // 表示領域
        const int res_x;            // ディスプレイの縦の長さ
        const int res_y;            // ディスプレイの横の長さ
        
    public:
        FrameDecoder(const msgbuf_ptr_t rbuf, const framebuf_ptr_t vbuf, const cv::Rect roi, const int res_x, const int res_y);  // コンストラクタ
        void run();  // フレーム展開を開始
};

#endif  /* FRAME_DECOMPRESSER_HPP */

