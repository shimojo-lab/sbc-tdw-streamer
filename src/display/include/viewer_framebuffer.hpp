/******************************
*   viewer_framebuffer.hpp   *
*   (表示フレームバッファ)   *
******************************/

#ifndef VIEWER_FRAMEBUFFER_HPP
#define VIEWER_FRAMEBUFFER_HPP

#include "mutex_logger.hpp"
#include <mutex>
#include <condition_variable>
#include <memory>
#include <cstdlib>
#include <cstring>
extern "C"{
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #include <sys/mman.h>
    #include <linux/fb.h>
    #include <linux/kd.h>
}

const int DEVICE_OPEN_FAILED = -1;  // デバイスファイルオープン失敗時の返り値
const int BITS_PER_PIXEL = 24;      // 1ピクセルあたりのビット数

/* 表示フレームバッファ */
class ViewerFramebuffer{
    private:
        int fb;                                      // フレームバッファ
        int tty;                                     // デバイス端末
        int fb_size;                                 // フレームバッファの領域サイズ
        int total_fb_size;                           // フレームバッファの合計サイズ
        unsigned char *fb_ptr;                       // フレームバッファの先頭
        fb_var_screeninfo vinfo;                     // フレームバッファの設定
        const int viewbuf_size;                      // フレームバッファの領域数
        int cur_page = 0;                            // フレームバッファの表示領域
        int stored_num = 0;                          // 格納済みフレーム数
        mutable std::mutex lock;                     // 排他制御用ロック
        mutable std::condition_variable empty_wait;  // 条件変数
        mutable std::condition_variable full_wait;   // 条件変数
        
        const bool openFramebuffer(const std::string fb_dev,  // フレームバッファをオープン
                                   const int width, const int height);
        const bool hideCursor(const std::string tty_dev);     // カーソルを非表示化
    
    public:
        ViewerFramebuffer(const std::string fb_dev, const int width,  // コンストラクタ
                          const int height, const int viewbuf_size, const std::string tty_dev);
        ~ViewerFramebuffer();                         // デストラクタ
        unsigned char *getFramebuffer(const int id);  // フレームバッファの描画領域を取得
        void activateFramebuffer();                   // フレームバッファを有効化
        void waitForFramebuffer();                    // フレームが供給されるまで待機
        void swapFramebuffer();                       // フレームバッファの表示領域を切り替え
};

using rawbuf_ptr_t = std::shared_ptr<ViewerFramebuffer>;

#endif  /* VIEWER_FRAMEBUFFER_HPP */

