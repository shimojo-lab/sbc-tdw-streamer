/****************************************
*           frame_encoder.cpp           *
*  (the JPEG encoder for video frames)  *
****************************************/

#ifndef FRAME_ENCODER_HPP
#define FRAME_ENCODER_HPP

#include "mutex_logger.hpp"
#include "sync_utils.hpp"
#include "transceive_framebuffer.hpp"
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
extern "C"{
    #include <turbojpeg.h>
}

const int COLOR_CHANNEL_NUM = 3;  // the number of the color channels
const int JPEG_FAILED = -1;       // the return value in failing JPEG encode

/* the JPEG encoder for video frames */
class FrameEncoder{
    private:
        const tjhandle handle;                  // the TurboJPEG encoder
        cv::VideoCapture video;                 // the video
        const int display_num;                  // the number of the displays
        double ratio;                           // the resize ratio
        int interpolation_type;                 // the method for resizing
        cv::Mat resized_frame;                  // a resized frame
        cv::Rect roi;                           // the area to paste a resized frame
        jpeg_params_t& ycbcr_format_list;       // the YCbCr formats applied for the display nodes
        jpeg_params_t& quality_list;            // the quality factors applied for the display nodes
        std::vector<cv::Rect> regions;          // the areas displayed by the display nodes
        std::vector<cv::Mat> raw_frames;        // raw frames sended to the display nodes
        std::vector<tranbuf_ptr_t>& send_bufs;  // the send framebuffer
        
        void setResizeParams(const int column, const int row,       // set the parameters for resizing a frame
                             const int bezel_w, const int bezel_h,
                             const int width, const int height,
                             const int frame_w, const int frame_h);
        void resize(cv::Mat& video_frame);                          // resize a frame
        void encode(const int id);                                  // encode a frame
        
    public:
        FrameEncoder(const std::string src, const int column, const int row,  // constructor
                     const int bezel_w, const int bezel_h, const int width,
                     const int height, jpeg_params_t& ycbcr_format_list, jpeg_params_t& quality_list,
                     std::vector<tranbuf_ptr_t>& send_bufs);
        ~FrameEncoder();  // destructor
        void run();       // start encoding video frames
};

#endif  /* FRAME_ENCODER_HPP */

