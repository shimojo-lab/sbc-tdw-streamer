/** main.cpp **/

#include "main.hpp"
#include <iostream>
#include <cstring>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <SDL2/SDL.h>

/* Main関数 */
int main(){
	cv::Mat img = cv::imread("./src/client/a.jpg", 1);
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture * texture;
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cerr << "Error" << SDL_GetError() << std::endl;
	}else{
		window = SDL_CreateWindow("test",
				          SDL_WINDOWPOS_UNDEFINED,
					  SDL_WINDOWPOS_UNDEFINED,
					  720,
					  480,
					  SDL_WINDOW_RESIZABLE);
		if(window == NULL){
			std::cerr << SDL_GetError() << std::endl;
		}else{
			renderer = SDL_CreateRenderer(window, -1, 0);
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, 720, 480);
			SDL_SetRenderTarget(renderer, texture);
			SDL_RenderClear(renderer);
			unsigned char *bytes;
			int32_t pitch;
			SDL_LockTexture(texture, nullptr, reinterpret_cast<void **>(&bytes), &pitch);
			SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_BGRA8888);
			for(int j=0; j<img.rows; ++j){
				cv::Vec3b *data = img.ptr<cv::Vec3b>(j);
				for(int i=0; i<img.cols; ++i){
					*bytes++ = 255;
					*bytes++ = data[i][2];
					*bytes++ = data[i][1];
					*bytes++ = data[i][0];
				}
			}
			SDL_UnlockTexture(texture);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			SDL_Delay(3000);
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

