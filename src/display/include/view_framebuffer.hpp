/******************************
*    view_framebuffer.hpp    *
*   (表示フレームバッファ)   *
******************************/

#ifndef VIEW_FRAMEBUFFER_HPP
#define VIEW_FRAMEBUFFER_HPP

#include <vector>
#include <memory>
#include <atomic>
#include <chrono>
#include <thread>

const int COLOR_CHANNEL_NUM = 3;          // 色のチャネル数
const int VIEWBUF_SPINLOCK_INTERVAL = 1;  // スピンロック中の待機時間

/* 表示フレームバッファ */
class ViewFramebuffer{
    private:
        const int page_num;                          // バッファ領域数
        std::vector<unsigned char*> page_ptrs;       // バッファ領域
        std::vector<std::atomic<bool>> page_states;  // バッファ領域の管理フラグ
        int cur_page = 0;                            // 表示するバッファ領域
    
    public:
        ViewFramebuffer(const int width, const int height,  // コンストラクタ
                        const int page_num);
        ~ViewFramebuffer();                                 // デストラクタ
        unsigned char *getDrawPage(const int id);           // 描画するバッファ領域を取得
        const unsigned char *getDisplayPage();              // 表示するバッファ領域を取得
        const int getCurrentPage();                         // 表示するバッファ領域の番号を取得
        void activatePage(const int id);                    // バッファ領域の表示を有効化
        void deactivatePage();                              // バッファ領域の表示を無効化
};

using viewbuf_ptr_t = std::shared_ptr<ViewFramebuffer>;

#endif  /* VIEW_FRAMEBUFFER_HPP */

