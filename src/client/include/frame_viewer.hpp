/*******************************
*       frame_viewer.hpp       *
*   (フレーム表示モジュール)   *
*******************************/

#ifndef FRAME_VIEWER_HPP
#define FRAME_VIEWER_HPP

#include <iostream>
#include <cstdlib>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <SDL2/SDL.h>

/* 定数の定義 */
extern const char* const APP_NAME;  // プログラム名
extern const int ALPHA;             // アルファチャンネル

/* フレーム表示モジュール */
class FrameViewer{
    private:
        int width;               // フレームの横の長さ
        int height;              // フレームの縦の長さ
        SDL_Window *window;      // SDLのウィンドウ
        SDL_Renderer *renderer;  // SDLのレンダラー
        SDL_Texture *texture;    // SDLのテクスチャ
    public:
        FrameViewer(int res_x, int res_y);         // コンストラクタ
        ~FrameViewer();                            // デストラクタ
        void displayFrame(std::vector<unsigned char> &comp_buf);  // フレームを表示
};

#endif

