/********************************
 *       frame_viewer.cpp      *
 *   (フレーム表示モジュール)  *
 *******************************/

#include "frame_viewer.hpp"

/* 定数の定義 */
const char* const APP_NAME = "sbc-tdw-streamer";  // プログラム名
const int ALPHA = 255;                            // アルファチャンネル値

/* コンストラクタ */
FrameViewer::FrameViewer(int width, int height):
    width(width),
    height(height)
{
    // SDLを起動
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "[Error] SDL_Init failed. (" << SDL_GetError() << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    // SDLのウィンドウを起動
    this->window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    this->width, this->height, SDL_WINDOW_FULLSCREEN);
    if(this->window == NULL){
        std::cerr << "[Error] SDL_CreateWindow failed. (" << SDL_GetError() << ")" << std::endl;
        SDL_Quit();
        std::exit(EXIT_FAILURE);
    }
    
    // SDLのレンダラーとテクスチャを初期化
    this->renderer = SDL_CreateRenderer(this->window, -1, 0);
    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_BGRA8888,
                                      SDL_TEXTUREACCESS_STREAMING, this->width, this->height);
    SDL_SetRenderTarget(this->renderer, this->texture);
    SDL_RenderClear(this->renderer);
}

/* デストラクタ */
FrameViewer::~FrameViewer(){
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

/* フレームを表示するメソッド */
void FrameViewer::displayFrame(std::vector<unsigned char> &comp_buf){
    // フレームを展開
    cv::Mat frame = cv::imdecode(comp_buf, CV_LOAD_IMAGE_UNCHANGED);
    //cv::resize(frame, frame, cv::Size(), 1920/frame.cols, 1080/frame.rows);
cv::imshow("", frame);
cv::waitKey(10);
    // テクスチャをロック
/*    unsigned char *pixel_addr;
    int32_t pitch;
    SDL_LockTexture(this->texture, nullptr, reinterpret_cast<void **>(&pixel_addr), &pitch);
	
	// A,R,G,Bの順に輝度値を書き込み
    for(int y=0; y<this->height; ++y){
        cv::Vec3b *data = frame.ptr<cv::Vec3b>(y);
        for(int x=0; x<this->width; ++x){
            *pixel_addr++ = ALPHA;
            *pixel_addr++ = data[x][2];
            *pixel_addr++ = data[x][1];
            *pixel_addr++ = data[x][0];
        }
    }
    
    // テクスチャをアンロックしてウィンドウに表示
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
    SDL_RenderPresent(this->renderer);
    SDL_Delay(10);
*/    return;
}

