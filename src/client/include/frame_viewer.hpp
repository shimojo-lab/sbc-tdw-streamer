/******************************
 *      frame_viewer.hpp      *
 *    (分割フレーム表示器)    *
 ******************************/

#ifndef FRAME_VIEWER_HPP
#define FRAME_VIEWER_HPP

#include "frame_queue.hpp"
#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

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

using smt_window_t = std::unique_ptr<SDL_Window, sdl_deleter_t>;      // SDLウィンドウのuniqueポインタ
using smt_renderer_t = std::unique_ptr<SDL_Renderer, sdl_deleter_t>;  // SDLレンダラーのuniqueポインタ
using smt_texture_t = std::unique_ptr<SDL_Texture, sdl_deleter_t>;    // SDLテクスチャのuniqueポインタ

/* 分割フレーム表示部 */
class FrameViewer{
    private:
        const int res_x;          // ディスプレイの縦の長さ
        const int res_y;          // ディスプレイの横の長さ
        const int width;          // フレームの横の長さ
        const int height;         // フレームの縦の長さ
        const smt_fq_t queue;     // 分割フレームキュー
        smt_window_t window;      // SDLウィンドウ
        smt_renderer_t renderer;  // SDLレンダラー
        smt_texture_t texture;    // SDLテクスチャ
        const bool createWindow(const char* const title);  // SDLウィンドウを初期化
        const bool createRenderer();                       // SDLレンダラーを初期化
        const bool createTexture();                        // SDLテクスチャを初期化
        cv::Mat decompressFrame();                         // フレームを展開
        void displayFrame(cv::Mat frame);                  // フレームを表示
    public:
        FrameViewer(const char* const title, const int res_x, const int res_y, const int width, const int height, const smt_fq_t queue);  // コンストラクタ
        ~FrameViewer();  // デストラクタ
        void start();    // フレーム表示を開始
};

#endif  /* FRAME_VIEWER_HPP */

