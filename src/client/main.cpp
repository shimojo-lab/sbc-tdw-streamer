/** main.cpp **/

#include "main.hpp"

/* Main関数 */
int main(){
    asio::io_service ios;
    FrameReciever receiver(&ios, 11111);
    ios.run();
receiver.waitForConnection();
	//cv::Mat img = cv::imread("./src/client/a.jpg", 1);
	//FrameViewer viewer(720, 480);
	//viewer.displayFrame(img);
	return 0;
}

