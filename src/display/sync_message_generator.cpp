/****************************************************
*            sync_message_generator.cpp             *
*  (message generator for synchronization process)  *
****************************************************/

#include "sync_message_generator.hpp"

/* constructor */
SyncMessageGenerator::SyncMessageGenerator(const int target_fps, const double fps_jitter,
                                           const int tuning_term, const tranbuf_ptr_t recv_buf,
                                           const int ycbcr_format, const int quality):
    tuning_term(tuning_term),
    recv_buf(recv_buf),
    ycbcr_format(ycbcr_format),
    quality(quality),
    min_available_t(1000.0/(double)(target_fps+fps_jitter)),
    max_available_t(1000.0/(double)(target_fps-fps_jitter))
{}

/* change the YCbCr format */
void SyncMessageGenerator::changeYCbCr(const int change_flag){
    switch(change_flag){
    case JPEG_PARAM_UP:
        switch(this->ycbcr_format){
        case TJSAMP_422:
            this->ycbcr_format = TJSAMP_444;
            break;
        case TJSAMP_420:
            this->ycbcr_format = TJSAMP_422;
            break;
        }
        break;
    case JPEG_PARAM_DOWN:
        switch(this->ycbcr_format){
        case TJSAMP_444:
            this->ycbcr_format = TJSAMP_422;
            break;
        case TJSAMP_422:
            this->ycbcr_format = TJSAMP_420;
            break;
        }
        break;
    }
}

/* change the quality factor */
void SyncMessageGenerator::changeQuality(const int change_flag){
    switch(change_flag){
    case JPEG_PARAM_UP:
        ++this->quality;
        break;
    case JPEG_PARAM_DOWN:
        --this->quality;
        break;
    }
}

/* check the decode speed */
const bool SyncMessageGenerator::checkDecodeSpeed(int& param_flag, int& change_flag){
    // measure the elapsed time in a term
    const double wait_t = this->wait_t_sum / (double)this->tuning_term;
    const double sync_t = this->sync_t_sum / (double)this->tuning_term;
    const double view_t = this->view_t_sum / (double)this->tuning_term;
    const double min_wait_t = this->min_available_t - sync_t - view_t;
    const double max_wait_t = this->max_available_t - sync_t - view_t;
    
    // decide the tuning method of the JPEG parameters
    bool result = false;
    if(wait_t > max_wait_t){
        result = true;
        change_flag = JPEG_PARAM_DOWN;
        if(this->ycbcr_format != TJSAMP_420){
            param_flag = JPEG_YCbCr_CHANGE;
            this->changeYCbCr(change_flag);
        }else if(this->quality > JPEG_QUALITY_MIN){
            param_flag = JPEG_QUALITY_CHANGE;
            this->changeQuality(change_flag);
        }
    }else if(wait_t < min_wait_t){
        result = true;
        change_flag = JPEG_PARAM_UP;
        if(this->quality < JPEG_QUALITY_MAX){
            param_flag = JPEG_QUALITY_CHANGE;
            this->changeQuality(change_flag);
        }else if(this->ycbcr_format != TJSAMP_444){
            param_flag = JPEG_YCbCr_CHANGE;
            this->changeYCbCr(change_flag);
        }
    }
    
    // reset the measured time
    this->wait_t_sum = 0.0;
    this->sync_t_sum = 0.0;
    this->view_t_sum = 0.0;
    return result;
}

/* generate a sync message */
const std::string SyncMessageGenerator::generate(){
    int param_flag = JPEG_NO_CHANGE;
    int change_flag = JPEG_PARAM_KEEP;
    ++this->frame_count;
    
    // decide the tuning method
    if(this->frame_count == this->tuning_term){
        this->checkDecodeSpeed(param_flag, change_flag);
        this->frame_count = 0;
    }
    
    // serialize the sync message
    this->sync_params.setIntParam("param", param_flag);
    this->sync_params.setIntParam("change", change_flag);
    return this->sync_params.serialize();
}

