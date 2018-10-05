#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

/* Main */
int main(int argc, char* argv[]){
    cv::VideoCapture cap("a.mp4");
    if(!cap.isOpened()){
        std::cout << "read error." << std::endl;
        return -1;
    }
    
    cv::Mat frame;
    int num = 0;
    for(;;){
        cap >> frame;
        num++;
        std::cout << num << std::endl;
    }
    return 0;
}

