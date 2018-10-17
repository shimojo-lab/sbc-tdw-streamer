/********************************
 *       frame_viewer.hpp      *
 *   (フレーム表示モジュール)  *
 *******************************/

#ifndef FRAME_VIEWER_HPP
#define FRAME_VIEWER_HPP

#include <iostream>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <SDL2/SDL.h>

#define PROGRAM_NAME "pi_tdw_streamer"
#define ALPHA_VALUE  255

/* フレーム表示モジュール */
class FrameViewer{
    private:
        int width;               // フレームの横の長さ
        int height;              // フレームの縦の長さ
        SDL_Window *window;      // SDLのウィンドウ
        SDL_Renderer *renderer;  // SDLのレンダラー
        SDL_Texture *texture;    // SDLのテクスチャ
    public:
        FrameViewer(int res_x, int res_y);        // コンストラクタ
        ~FrameViewer();                           // デストラクタ
        inline void displayFrame(cv::Mat frame);  // フレームを表示
};

#endif

