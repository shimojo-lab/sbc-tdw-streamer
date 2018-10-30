/******************************
 *      frame_viewer.cpp      *
 *    (分割フレーム表示器)    *
 ******************************/

#include "frame_viewer.hpp"

const int SDL_STATUS_GREEN = 0;   // SDL正常起動時のステータスコード
const int SDL_DRIVER_INDEX = -1;  // SDLレンダラーのドライバ指定
const int SDL_DRIVER_FLAG = 0;    // SDLレンダラーのフラグ
const int SDL_ALPHA_CH = 255;     // アルファチャンネル値

/* コンストラクタ */
FrameViewer::FrameViewer(const char* const title, const int res_x, const int res_y, const int width, const int height, const smt_fq_t queue):
    res_x(res_x),
    res_y(res_y),
    width(width),
    height(height),
    queue(queue)
{/*
    // SDLを初期化
    if(SDL_Init(SDL_INIT_VIDEO) != SDL_STATUS_GREEN){
        std::cerr << "[Error] SDL_Init failed. (" << SDL_GetError() << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::atexit(SDL_Quit);
    
    // SDLウィンドウを初期化
    if(!this->createWindow(title)){
        std::exit(EXIT_FAILURE);
    }
    
    // SDLレンダラーを初期化
    if(!this->createRenderer()){
        std::exit(EXIT_FAILURE);
    }
    
    // SDLテクスチャを初期化
    if(!this->createTexture()){
        std::exit(EXIT_FAILURE);
    }*/
}

/* SDLウィンドウを初期化 */
const bool FrameViewer::createWindow(const char* const title){
    sdl_deleter_t deleter;
    this->window = smt_window_t(
        SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->res_x, this->res_y, SDL_WINDOW_FULLSCREEN),
        deleter
    );
    if(this->window == NULL){
        std::cerr << "[Error] SDL_CreateWindow failed. (" << SDL_GetError() << ")" << std::endl;
        return false;
    }else{
        return true;
    }
}

/* SDLレンダラーを初期化 */
const bool FrameViewer::createRenderer(){
    sdl_deleter_t deleter;
    this->renderer = smt_renderer_t(
        SDL_CreateRenderer(this->window.get(), SDL_DRIVER_INDEX, SDL_DRIVER_FLAG),
        deleter
    );
    if(this->renderer == NULL){
        std::cerr << "[Error] SDL_CreateRenderer failed. (" << SDL_GetError() << ")" << std::endl;
        return false;
    }else{
        return true;
    }
}

/* SDLテクスチャを初期化 */
const bool FrameViewer::createTexture(){
    sdl_deleter_t deleter;
    this->texture = smt_texture_t(
        SDL_CreateTexture(this->renderer.get(), SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_TARGET, this->width, this->height),
        deleter
    );
    if(this->texture == NULL){
        std::cerr << "[Error] SDL_CreateTexture failed. (" << SDL_GetError() << ")" << std::endl;
        return false;
    }
    
    if(SDL_SetRenderTarget(this->renderer.get(), this->texture.get()) != SDL_STATUS_GREEN){
        std::cerr << "[Error] SDL_RenderTarget failed. (" << SDL_GetError() << ")" << std::endl;
        return false;
    }
    if(SDL_RenderClear(this->renderer.get()) != SDL_STATUS_GREEN){
        std::cerr << "[Error] SDL_RenderClear failed. (" << SDL_GetError() << ")" << std::endl;
        return false;
    }
    return true;
}

/* フレームを展開するメソッド */
cv::Mat FrameViewer::decompressFrame(){
    // フレームを展開
    cv::Mat frame = cv::imdecode(this->queue->dequeue(), CV_LOAD_IMAGE_UNCHANGED);
    return frame;
}

/* フレームを表示するメソッド */
void FrameViewer::displayFrame(cv::Mat frame){
try{
    cv::imshow("", frame);
    cv::waitKey(10);
}catch(...){
    std::cerr << "jpeg failed." << std::endl;
}
/*    // テクスチャをロック
    unsigned char *pixel_addr;
    int32_t pitch;
    SDL_LockTexture(this->texture.get(), nullptr, reinterpret_cast<void **>(&pixel_addr), &pitch);
	
	// A,R,G,Bの順に輝度値を書き込み
    for(int y=0; y<this->height; ++y){
        cv::Vec3b *data = frame.ptr<cv::Vec3b>(y);
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
    SDL_Delay(10);
*/    return;
}

/* フレーム表示を開始するメソッド */
void FrameViewer::start(){
    for(int i=0; i<10000; ++i){
        this->displayFrame(this->decompressFrame());
    }
    return;
}

