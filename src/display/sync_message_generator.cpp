/*********************************
*   sync_message_generator.cpp   *
*     (同期メッセージ生成器)     *
*********************************/

#include "sync_message_generator.hpp"

/* コンストラクタ */
SyncMessageGenerator::SyncMessageGenerator(const int target_fps, const int tuning_term,
                                           const tranbuf_ptr_t recv_buf, const int sampling_type,
                                           const int quality):
    tuning_term(tuning_term),
    recv_buf(recv_buf),
    sampling_type(sampling_type),
    quality(quality),
    available_time(1000.0/(double)target_fps)
{}

/* 品質係数を変更 */
void SyncMessageGenerator::tuneQuality(int& param_flag, int& change_flag, const int type){
    param_flag = JPEG_QUALITY_CHANGE;
    if(type == JPEG_PARAM_DOWN){
        change_flag = JPEG_PARAM_DOWN;
        --this->quality;
    }else{
        change_flag = JPEG_PARAM_UP;
        ++this->quality;
    }
}

/* クロマサブサンプル比を変更 */
void SyncMessageGenerator::tuneSamplingType(int& param_flag, int& change_flag, const int type){
    param_flag = JPEG_SAMPLING_CHANGE;
    if(type == JPEG_PARAM_DOWN){
        change_flag = JPEG_PARAM_DOWN;
        switch(this->sampling_type){
        case TJSAMP_444:
            this->sampling_type = TJSAMP_422;
            break;
        case TJSAMP_422:
            this->sampling_type = TJSAMP_420;
            break;
        }
    }else{
        change_flag = JPEG_PARAM_UP;
        switch(this->sampling_type){
        case TJSAMP_422:
            this->sampling_type = TJSAMP_444;
            break;
        case TJSAMP_420:
            this->sampling_type = TJSAMP_422;
            break;
        }
    }
}

/* パラメータ変更要求の内容を決定 */
void SyncMessageGenerator::decideTuningRequest(int& param_flag, int& change_flag){
    if(!dec_tuned){  // 表示バッファ供給速度をチューニング
        const double wait_t = this->wait_t_sum / (double)this->tuning_term;
        const double sync_t = this->sync_t_sum / (double)this->tuning_term;
        const double view_t = this->view_t_sum / (double)this->tuning_term;
        const double idle_wait_t = this->available_time - sync_t - view_t;
        
        if(wait_t>idle_wait_t){
            const double diff_t = this->pre_wait_t - wait_t;
            const double expected_wait_t = wait_t - diff_t * (double)(this->quality-1);
            if(this->quality!=JPEG_QUALITY_MIN && expected_wait_t<idle_wait_t){
                this->tuneQuality(param_flag, change_flag, JPEG_PARAM_DOWN);
            }else if(this->sampling_type != TJSAMP_420){
                this->tuneSamplingType(param_flag, change_flag, JPEG_PARAM_DOWN);
            }
            this->pre_wait_t = wait_t;
            this->wait_t_sum = 0.0;
            this->sync_t_sum = 0.0;
            this->view_t_sum = 0.0; 
        }else{
            this->dec_tuned = true;
            this->best_quality = this->quality;
            this->best_sampling_type = this->sampling_type;
            this->pre_recvbuf_stored = this->recv_buf->getStoredNum();
        }
    }else{  // 受信バッファ供給速度をチューニング
        const int recvbuf_stored = this->recv_buf->getStoredNum();
        const int diff_stored = recvbuf_stored - this->pre_recvbuf_stored;
        
        if(recvbuf_stored==0 || diff_stored<1){
            if(this->sampling_type != TJSAMP_420){
                this->tuneSamplingType(param_flag, change_flag, JPEG_PARAM_DOWN);
            }else if(this->quality != JPEG_QUALITY_MIN){
                this->tuneQuality(param_flag, change_flag, JPEG_PARAM_DOWN);
            }
        }else{
            if(this->quality != this->best_quality){
                this->tuneQuality(param_flag, change_flag, JPEG_PARAM_UP);
            }else if(this->sampling_type != this->best_sampling_type){
                this->tuneSamplingType(param_flag, change_flag, JPEG_PARAM_UP);
            }
        }
        this->pre_recvbuf_stored = recvbuf_stored;
    }
}

/* 同期メッセージを生成 */
const std::string SyncMessageGenerator::generate(){
    // 変更要求の内容を決定
    int param_flag = JPEG_NO_CHANGE;
    int change_flag = JPEG_PARAM_KEEP;
    ++this->frame_count;
    if(this->frame_count == this->tuning_term){
        this->decideTuningRequest(param_flag, change_flag);
        this->frame_count = 0;
    }
    
    // JSON形式でシリアライズ
    this->sync_params.setIntParam("param", param_flag);
    this->sync_params.setIntParam("change", change_flag);
    return this->sync_params.serialize();
}

