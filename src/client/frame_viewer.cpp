/** frame_viewer.cpp **/

#include "frame_viewer.hpp"

// コンストラクタ
FrameViewer::FrameViewer(int width, int height){
	// パラメータを設定
	this->width = width;
	this->height = height;
	
	// SDLを初期化
	if(this->initSDL() == false){
		exit(-1);
	}
}

bool FrameViewer::initSDL(){
	// SDLを起動
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cerr << "[Error] Could not initialize SDL (" << ")" << SDL_GetError() << std::endl;
		return false;
	}
	
	// SDLのウィンドウを起動
	this->window = SDL_CreateWindow("pi_tdw_streamer",
			                SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					this->width,
					this->height,
					SDL_WINDOW_FULLSCREEN);
	if(this->window == NULL){
		std::cerr << "[Error] Could not open window (" << SDL_GetError() << ")" << std::endl;
		SDL_Quit();
		return false;
	}
	
	// SDLのレンダラーとテクスチャを初期化
	this->renderer = SDL_CreateRenderer(this->window, -1, 0);
	this->texture = SDL_CreateTexture(this->renderer,
					  SDL_PIXELFORMAT_BGRA8888,
					  SDL_TEXTUREACCESS_STREAMING,
					  this->width,
					  this->height);
	SDL_SetRenderTarget(this->renderer, this->texture);
	SDL_RenderClear(this->renderer);
	return true;
}

// デストラクタ
FrameViewer::~FrameViewer(){
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
	SDL_Quit();
}

// フレームを表示するメソッド
void FrameViewer::displayFrame(cv::Mat frame){
	// テクスチャをロック
	unsigned char *pixel_addr;
	int32_t pitch;
	SDL_LockTexture(this->texture,
			nullptr, 
			reinterpret_cast<void **>(&pixel_addr),
			&pitch);
	
	// ARGBの順に書き込み
	for(int y=0; y<this->height; ++y){
		cv::Vec3b *data = frame.ptr<cv::Vec3b>(y);
		for(int x=0; x<this->width; ++x){
			*pixel_addr++ = 255;
			*pixel_addr++ = data[x][2];
			*pixel_addr++ = data[x][1];
			*pixel_addr++ = data[x][0];
		}
	}
	
	// テクスチャをアンロックしてウィンドウに表示
	SDL_UnlockTexture(texture);
	SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
	SDL_RenderPresent(this->renderer);
	SDL_Delay(1000);
	return;
}

