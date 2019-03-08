/******************************************************
*             sync_message_generator.hpp              *
*  (the generator of messages used as sync messages)  *
******************************************************/

#ifndef SYNC_MESSAGE_GENERATOR_HPP
#define SYNC_MESSAGE_GENERATOR_HPP

#include "json_handler.hpp"
#include "transceive_framebuffer.hpp"
#include "sync_utils.hpp"
extern "C"{
    #include <turbojpeg.h>
}

/* the geneator of messages used as sync messages */
class SyncMessageGenerator{
    private:
        JsonHandler sync_params;       // the parameters packed in a sync message
        const int tuning_term;         // the term of tuning JPEG parameters
        const tranbuf_ptr_t recv_buf;  // the receive framebuffer
        int ycbcr_format;              // the YCbCr format
        int quality;                   // the quality factor
        int frame_count = 0;           // the number of obsoleted frames
        const double min_available_t;  // the minimum time for decoding a frame
        const double max_available_t;  // the maximum time for decoding a frame
        
        void changeYCbCr(const int change_flag);                         // change the YCbCr format
        void changeQuality(const int change_flag);                       // chnage the quality factor
        const bool checkDecodeSpeed(int& param_flag, int& change_flag);  // check the decode speed
    
    public:
        double wait_t_sum = 0.0;         // the elapsed time in decoding a frame
        double sync_t_sum = 0.0;         // the elapsed time in the synchronization process
        double view_t_sum = 0.0;         // the elapsed time in displaying a frame
        
        SyncMessageGenerator(const int target_fps, const int tuning_term,  // constructor
                             const tranbuf_ptr_t recv_buf, const int ycbcr_format,
                             const int quality, const double fps_jitter);
        const std::string generate();                                      // generate a sync message
};

#endif  /* SYNC_MESSAGE_GENERATOR_HPP */

