/********************************
 *      sdl2_wrapper.cpp        *
 *    (SDL2のC+11用ラッパー)    *
 *******************************/

#include "sdl2_wrapper.hpp"

const std::string APP_NAME("sbc-tdw-streamer");  // アプリケーション名
const int SDL_STATUS_GREEN = 0;                  // SDL正常起動時のステータスコード
const int SDL_DRIVER_INDEX = -1;                 // SDLレンダラーのドライバ指定
const int SDL_DRIVER_FLAG = 0;                   // SDLレンダラーのフラグ
const int SDL_ALPHA_CH = 255;                    // アルファチャンネル値

/* コンストラクタ */
SDL2Wrapper::SDL2Wrapper(const int res_x, const int res_y, const int width, const int height, const int framerate):
    width(width),
    height(height)
{    // SDLを初期化
    if(SDL_Init(SDL_INIT_VIDEO) != SDL_STATUS_GREEN){
        print_err("Failed to initialize SDL", SDL_GetError());
        std::exit(EXIT_FAILURE);
    }
    std::atexit(SDL_Quit);
    
    // SDLウィンドウを初期化
    if(!this->createWindow(res_x, res_y)){
        std::exit(EXIT_FAILURE);
    }
    
    // SDLレンダラーを初期化
    if(!this->createRenderer()){
        std::exit(EXIT_FAILURE);
    }
    
    // SDLテクスチャを初期化
    if(!this->createTexture()){
        std::exit(EXIT_FAILURE);
    }
    
    // フレーム表示時間を設定
    this->view_time = 1000.0 / (double)framerate;
}

/* SDLウィンドウを初期化 */
const bool SDL2Wrapper::createWindow(const int res_x, const int res_y){
    sdl_deleter_t deleter;
    this->window = win_ptr_t(SDL_CreateWindow(APP_NAME.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, res_x, res_y, SDL_WINDOW_FULLSCREEN), deleter);
    if(window == NULL){
        print_err("Failed to create SDL window", SDL_GetError());
        return false;
    }else{
        return true;
    }
}

/* SDLレンダラーを初期化 */
const bool SDL2Wrapper::createRenderer(){
    sdl_deleter_t deleter;
    this->renderer = ren_ptr_t(SDL_CreateRenderer(this->window.get(), SDL_DRIVER_INDEX, SDL_DRIVER_FLAG), deleter);
    if(this->renderer == NULL){
        print_err("Failed to create SDL renderer", SDL_GetError());
        return false;
    }else{
        return true;
    }
}

/* SDLテクスチャを初期化 */
const bool SDL2Wrapper::createTexture(){
    sdl_deleter_t deleter;
    this->texture = tex_ptr_t(SDL_CreateTexture(this->renderer.get(), SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, this->width, this->height), deleter);
    if(this->texture == NULL){
        print_err("Failed to create SDL texture", SDL_GetError());
        return false;
    }
    
/*    if(SDL_SetRenderTarget(this->renderer.get(), this->texture.get()) != SDL_STATUS_GREEN){
        print_err("Failed SDL_RenderTarget", SDL_GetError());
        return false;
    }*/
    
    if(SDL_RenderClear(this->renderer.get()) != SDL_STATUS_GREEN){
        print_err("Failed SDL_RenderClear", SDL_GetError());
        return false;
    }
    return true;
}

/* フレームを表示するメソッド */
void SDL2Wrapper::renderFrame(cv::Mat& frame){
    // テクスチャをロック
    unsigned char *pixel_addr;
    int32_t pitch;
    SDL_LockTexture(this->texture.get(), nullptr, reinterpret_cast<void **>(&pixel_addr), &pitch);
	
	// A,R,G,Bの順に輝度値を書き込み
    for(int y=0; y<this->height; ++y){
        const cv::Vec3b *data = frame.ptr<cv::Vec3b>(y);
        for(int x=0; x<this->width; ++x){
            *pixel_addr++ = SDL_ALPHA_CH;
            *pixel_addr++ = data[x][2];
            *pixel_addr++ = data[x][1];
            *pixel_addr++ = data[x][0];
        }
    }
    
    // テクスチャをアンロックしてウィンドウに表示
    SDL_UnlockTexture(texture.get());
    SDL_RenderCopy(this->renderer.get(), this->texture.get(), NULL, NULL);
    SDL_RenderPresent(this->renderer.get());
    SDL_Delay(this->view_time);
}

