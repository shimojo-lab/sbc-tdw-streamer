/*********************************
*   transceive_framebuffer.hpp   *
*    (送受信フレームバッファ)    *
*********************************/

#ifndef TRANSCEIVE_FRAMEBUFFER_HPP
#define TRANSCEIVE_FRAMEBUFFER_HPP

#include <mutex>
#include <thread>
#include <chrono>
#include <boost/circular_buffer.hpp>

const int BUF_FRONT_INDEX = 0;            // バッファの先頭のインデックス
const int TRANBUF_SPINLOCK_INTERVAL = 1;  // スピンロック中の待機時間

/* 送受信用フレームバッファ */
class TransceiveFramebuffer{
    private:
        boost::circular_buffer<std::string> jpeg_buf;  // フレームバッファの先頭
        std::mutex lock;                               // 排他制御用のロック
    
    public:
        TransceiveFramebuffer(const int jpegbuf_num);  // コンストラクタ
        void push(const std::string& jpeg_frame);      // フレームを格納
        const std::string pop();                       // フレームを取り出し
        const int getStoredNum();                      // 使用中の領域数を取得
};

using tranbuf_ptr_t = std::shared_ptr<TransceiveFramebuffer>;

#endif  /* TRANSCEIVE_FRAMEBUFFER_HPP */

