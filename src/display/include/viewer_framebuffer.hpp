/******************************
*   viewer_framebuffer.hpp   *
*   (表示フレームバッファ)   *
******************************/

#ifndef VIEWER_FRAMEBUFFER_HPP
#define VIEWER_FRAMEBUFFER_HPP

#include <memory>
#include <atomic>
extern "C"{
    #include <unistd.h>
}

/* 表示フレームバッファ */
class ViewerFramebuffer{
    private:
        const int viewbuf_num;        // フレームバッファの領域数
        const int viewbuf_size;       // フレームバッファのサイズ
        unsigned char *viewbuf_ptr;   // フレームバッファの先頭
        int cur_page = 0;             // 表示中のフレームバッファの領域
        std::atomic<int> stored_num;  // 格納済フレーム数
    
    public:
        ViewerFramebuffer(const int width, const int height,  // コンストラクタ
                          const int viewbuf_size);
        ~ViewerFramebuffer();                      // デストラクタ
        unsigned char *getDrawArea(const int id);  // フレームバッファの描画領域を取得
        unsigned char *getDisplayArea();           // フレームバッファの描画領域を取得
        void addFrameNum();                        // 格納済フレーム数を加算
        void subFrameNum();                        // 格納済フレーム数を減算
};

using rawbuf_ptr_t = std::shared_ptr<ViewerFramebuffer>;

#endif  /* VIEWER_FRAMEBUFFER_HPP */

