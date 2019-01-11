/*********************************
*   transceive_framebuffer.hpp   *
*   (送受信用フレームバッファ)   *
*********************************/

#ifndef TRANSCEIVE_FRAMEBUFFER_HPP
#define TRANSCEIVE_FRAMEBUFFER_HPP

#include <string>
#include <boost/circular_buffer.hpp>
extern "C"{
    #include <unistd.h>
}

const int BUF_FRONT_INDEX = 0;  // バッファの先頭のインデックス
const int VIEWBUF_ID_LEN = 1;   // フレーム表示バッファのインデックス長

/* 送受信用フレームバッファ */
class TransceiveFramebuffer{
    private:
        boost::circular_buffer<std::string> buf;  // リングバッファ
        const unsigned int wait_usec;             // スピンロックでの待機時間
    
    public:
        TransceiveFramebuffer(const int buf_num,  // コンストラクタ
                              const unsigned int wait_usec);
        void push(const std::string& frame);      // フレームを格納
        const std::string pop();                  // フレームを取り出し
        const int getItemNum();                   // 使用中の領域数を取得
};

using tranbuf_ptr_t = std::shared_ptr<TransceiveFramebuffer>;

#endif  /* TRANSCEIVE_FRAMEBUFFER_HPP */

