/******************************
*    view_framebuffer.hpp    *
*   (表示フレームバッファ)   *
******************************/

#ifndef VIEW_FRAMEBUFFER_HPP
#define VIEW_FRAMEBUFFER_HPP

#include <vector>
#include <memory>
#include <atomic>
extern "C"{
    #include <unistd.h>
}

const int COLOR_CHANNEL_NUM = 3;  // 色のチャネル数

/* 表示フレームバッファ */
class ViewFramebuffer{
    private:
        const int viewbuf_num;                     // フレームバッファの領域数
        const int frame_size;                      // フレームのサイズ
        const unsigned int wait_usec;              // スピンロック内の待機時間
        std::vector<unsigned char*> viewbuf_ptrs;  // フレームバッファの先頭
        int cur_page = 0;                          // 表示中のフレームバッファの領域
        std::atomic<int> stored_num;               // 格納済フレーム数
    
    public:
        ViewFramebuffer(const int width, const int height,  // コンストラクタ
                        const int viewbuf_num, const unsigned int wait_usec);
        ~ViewFramebuffer();                        // デストラクタ
        unsigned char *getDrawArea(const int id);  // フレームバッファの描画領域を取得
        const unsigned char *getDisplayArea();     // フレームバッファの描画領域を取得
        void addFrameNum();                        // 格納済フレーム数を加算
        void subFrameNum();                        // 格納済フレーム数を減算
};

using rawbuf_ptr_t = std::shared_ptr<ViewFramebuffer>;

#endif  /* VIEW_FRAMEBUFFER_HPP */

