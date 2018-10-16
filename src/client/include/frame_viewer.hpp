/** frame_viewer.hpp **/

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <SDL2/SDL.h>

/* フレーム表示部 */
class FrameViewer{
	private:
		int width;                // フレームの横の長さ
		int height;               // フレームの縦の長さ
		SDL_Window *window;       // SDLのウィンドウ
		SDL_Renderer *renderer;   // SDLのレンダラー
		SDL_Texture *texture;     // SDLのテクスチャ
	public:
		FrameViewer(int res_x, int res_y);  // コンストラクタ
		~FrameViewer();                     // デストラクタ
		bool initSDL();                     // SDLを初期化
		void displayFrame(cv::Mat frame);   // フレームを表示
};

