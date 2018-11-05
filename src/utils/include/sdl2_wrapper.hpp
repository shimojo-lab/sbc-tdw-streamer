/*******************************
 *      sdl2_wrapper.hpp       *
 *   (SDL2のC++11用ラッパー)   *
 ******************************/

#ifndef SDL2_WRAPPER_HPP
#define SDL2_WRAPPER_HPP

#include "print_with_mutex.hpp"
#include <SDL2/SDL.h>
#include <opencv2/core.hpp>

extern const std::string APP_NAME;  // アプリケーション名
extern const int SDL_STATUS_GREEN;  // SDL正常起動時のステータスコード
extern const int SDL_DRIVER_INDEX;  // SDLレンダラーのドライバ指定
extern const int SDL_DRIVER_FLAG;   // SDLレンダラーのフラグ
extern const int SDL_ALPHA_CH;      // アルファチャンネル値

/* SDLオブジェクト削除用の構造体 */
struct sdl_deleter_t{
    void operator()(SDL_Window *ptr){
        if(ptr){
            SDL_DestroyWindow(ptr);
        }
    }
    void operator()(SDL_Renderer *ptr){
        if(ptr){
            SDL_DestroyRenderer(ptr);
        }
    }
    void operator()(SDL_Texture *ptr){
        if(ptr){
            SDL_DestroyTexture(ptr);
        }
    }
};

using win_ptr_t = std::unique_ptr<SDL_Window, sdl_deleter_t>;
using ren_ptr_t = std::unique_ptr<SDL_Renderer, sdl_deleter_t>;
using tex_ptr_t = std::unique_ptr<SDL_Texture, sdl_deleter_t>;

/* SDL2のラッパークラス */
class SDL2Wrapper{
    private:
        const int width;     // フレームの横の長さ
        const int height;    // フレームの縦の長さ
        win_ptr_t window;    // SDLウィンドウ
        ren_ptr_t renderer;  // SDLレンダラー
        tex_ptr_t texture;   // SDLテクスチャ
        double view_time;    // フレーム表示時間
        
        const bool createWindow(const int res_x, const int res_y);  // SDLウィンドウを初期化
        const bool createRenderer();                                // SDLレンダラーを初期化
        const bool createTexture();                                 // SDLテクスチャを初期化
    public:
        SDL2Wrapper(const int res_x, const int res_y, const int width, const int height, const int framerate);  // コンストラクタ
        void renderFrame(cv::Mat& frame);
};

#endif  /* SDL2_WRAPPER_HPP */

