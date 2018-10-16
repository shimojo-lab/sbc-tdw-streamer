/** main.cpp **/

#include "main.hpp"

/* Main関数 */
int main(){
	cv::Mat img = cv::imread("./src/client/a.jpg", 1);
	FrameViewer viewer(720, 480);
	viewer.displayFrame(img);
	return 0;
}

