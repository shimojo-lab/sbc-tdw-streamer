/****************************************
*           frame_decoder.hpp           *
*  (the JPEG decoder for video frames)  *
****************************************/

#ifndef FRAME_DECODER_HPP
#define FRAME_DECODER_HPP

#include "mutex_logger.hpp"
#include "transceive_framebuffer.hpp"
#include "view_framebuffer.hpp"
extern "C"{
    #include <turbojpeg.h>
}
#ifdef DEBUG
#include "sync_utils.hpp"
#endif

const int JPEG_FAILED = -1;    // the return value in failing decoding JPEG
const int FRAME_ID_INDEX = 0;  // the initial frame index
const int FRAME_ID_LEN = 1;    // the length of the frame index

/* JPEG decoder for video frames */
class FrameDecoder{
    private:
        const tjhandle handle;         // the TruboJPEG decoder
        const tranbuf_ptr_t recv_buf;  // the receive framebuffer
        const viewbuf_ptr_t view_buf;  // the view framebuffer
        
        void decode(unsigned char *jpeg_frame, const unsigned long jpeg_size,  // decode a frame
                    const int id);
    
    public:
        FrameDecoder(const tranbuf_ptr_t recv_buf, const viewbuf_ptr_t view_buf);  // constructor
        ~FrameDecoder();  // destructor
        void run();       // start decoding JPEG frames
};

#endif  /* FRAME_DECODER_HPP */

