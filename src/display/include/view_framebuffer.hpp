/******************************
*    view_framebuffer.hpp    *
*   (表示フレームバッファ)   *
******************************/

#ifndef VIEW_FRAMEBUFFER_HPP
#define VIEW_FRAMEBUFFER_HPP

#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>

const int COLOR_CHANNEL_NUM = 3;          // 色のチャネル数
const int VIEWBUF_SPINLOCK_INTERVAL = 1;  // スピンロック中の待機時間

/* 表示フレームバッファ */
class ViewFramebuffer{
    private:
        const int viewbuf_num;                          // フレームバッファの領域数
        std::vector<unsigned char*> viewbuf_ptrs;       // フレームバッファの先頭
        std::vector<std::atomic<bool>> viewbuf_states;  // フレームバッファの使用状況
        int cur_page = 0;                               // フレームバッファの表示領域
    
    public:
        ViewFramebuffer(const int width, const int height,  // コンストラクタ
                        const int viewbuf_num);
        ~ViewFramebuffer();                                 // デストラクタ
        unsigned char *getDrawArea(const int id);           // フレームバッファの描画領域を取得
        const unsigned char *getDisplayArea();              // フレームバッファの表示領域を取得
        const int getCurrentPage();                         // 表示領域のインデックスを取得
        void activateFrame(const int id);                   // フレーム領域の表示を有効化
        void deactivateFrame();                             // フレーム領域の表示を無効化
};

using viewbuf_ptr_t = std::shared_ptr<ViewFramebuffer>;

#endif  /* VIEW_FRAMEBUFFER_HPP */

