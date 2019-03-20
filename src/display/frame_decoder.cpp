/************************************
*         frame_decoder.cpp         *
*  (JPEG decoder for video frames)  *
************************************/

#include "frame_decoder.hpp"

/* constructor */
FrameDecoder::FrameDecoder(const tranbuf_ptr_t recv_buf, const viewbuf_ptr_t view_buf):
    handle(tjInitDecompress()),
    recv_buf(recv_buf),
    view_buf(view_buf)
{
    // launch the TruboJPEG decoder
    if(this->handle == NULL){
        const std::string err_msg(tjGetErrorStr());
        _ml::caution("Failed to init JPEG decoder", err_msg);
        std::exit(EXIT_FAILURE);
    }
}

/* destructor */
FrameDecoder::~FrameDecoder(){
    tjDestroy(this->handle);
}

/* decode a JPEG frame */
void FrameDecoder::decode(unsigned char *jpeg_frame, const unsigned long jpeg_size, const int id){
    // read the header of the frame
    int frame_w, frame_h, sampling_type;
    const int tj_stat1 = tjDecompressHeader2(this->handle,
                                             jpeg_frame,
                                             jpeg_size,
                                             &frame_w,
                                             &frame_h,
                                             &sampling_type
    );
    if(tj_stat1 == JPEG_FAILED){
        const std::string err_msg(tjGetErrorStr());
        _ml::warn("Could not get new video frame", err_msg);
        return;
    }
    
    // decode the frame
    const int tj_stat2 = tjDecompress2(this->handle,
                                       jpeg_frame,
                                       jpeg_size,
                                       this->view_buf->getDrawPage(id),
                                       frame_w,
                                       frame_w*COLOR_CHANNEL_NUM,
                                       frame_h,
                                       TJPF_RGB,
                                       TJFLAG_FASTDCT|TJFLAG_FASTUPSAMPLE
    );
    if(tj_stat2 == JPEG_FAILED){
        const std::string err_msg(tjGetErrorStr());
        _ml::warn("Could not get new video frame", err_msg);
        return;
    }
}

/* start decoding JPEG frames */
void FrameDecoder::run(){
    while(true){
        std::string jpeg_str = this->recv_buf->pop();
        const int id = std::stoi(jpeg_str.substr(FRAME_ID_INDEX, FRAME_ID_LEN));
        jpeg_str.erase(FRAME_ID_INDEX, FRAME_ID_LEN);
        
        const unsigned long jpeg_size = (unsigned long)jpeg_str.length();
        std::vector<unsigned char> jpeg_frame(jpeg_str.c_str(), jpeg_str.c_str()+jpeg_size);
        this->decode(jpeg_frame.data(), jpeg_size, id);
        this->view_buf->activatePage(id);
    }
}

