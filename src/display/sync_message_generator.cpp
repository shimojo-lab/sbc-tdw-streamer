/*********************************
*   sync_message_generator.cpp   *
*     (同期メッセージ生成器)     *
*********************************/

#include "sync_message_generator.hpp"

/* コンストラクタ */
SyncMessageGenerator::SyncMessageGenerator(const int target_fps, const int tuning_term,
                                           const tranbuf_ptr_t recv_buf, const int yuv_format,
                                           const int quality):
    tuning_term(tuning_term),
    recv_buf(recv_buf),
    yuv_format(yuv_format),
    quality(quality),
    available_t(1000.0/(double)target_fps)
{}

/* YUVサンプル比を変更 */
void SyncMessageGenerator::switchYUV(const int change_flag){
    switch(change_flag){
    case JPEG_PARAM_UP:
        switch(this->yuv_format){
        case TJSAMP_422:
            this->yuv_format = TJSAMP_444;
            break;
        case TJSAMP_420:
            this->yuv_format = TJSAMP_422;
            break;
        }
        break;
    case JPEG_PARAM_DOWN:
        switch(this->yuv_format){
        case TJSAMP_444:
            this->yuv_format = TJSAMP_422;
            break;
        case TJSAMP_422:
            this->yuv_format = TJSAMP_420;
            break;
        }
        break;
    }
}

/* 品質係数を変更 */
void SyncMessageGenerator::switchQuality(const int change_flag){
    switch(change_flag){
    case JPEG_PARAM_UP:
        ++this->quality;
        break;
    case JPEG_PARAM_DOWN:
        --this->quality;
        break;
    }
}

/* フレーム展開速度を向上 */
void SyncMessageGenerator::speedUpDecode(int& param_flag, int& change_flag){
    const double wait_t = this->wait_t_sum / (double)this->tuning_term;
    const double sync_t = this->sync_t_sum / (double)this->tuning_term;
    const double view_t = this->view_t_sum / (double)this->tuning_term;
    const double idle_wait_t = this->available_t - sync_t - view_t;
    
    // 速度が不十分ならパラメータを低減
    if(wait_t > idle_wait_t){
        change_flag = JPEG_PARAM_DOWN;
        if(this->yuv_format != TJSAMP_420){
            param_flag = JPEG_YUV_CHANGE;
            this->switchYUV(change_flag);
        }else if(this->quality > JPEG_QUALITY_MIN){
            param_flag = JPEG_QUALITY_CHANGE;
            this->switchQuality(change_flag);
        }
    }else{
        // 現在のパラメータを最良値として採用
        this->tuning_mode = RECV_SPEED_CTRL;
        this->best_quality = this->quality;
        this->best_yuv_format = this->yuv_format;
        this->pre_recvbuf_stored = this->recv_buf->getStoredNum();
    }
    
    // 時間計測を再開
    this->wait_t_sum = 0.0;
    this->sync_t_sum = 0.0;
    this->view_t_sum = 0.0; 
}

/* フレーム展開速度を抑制 */
void SyncMessageGenerator::speedDownDecode(int& param_flag, int& change_flag){
    // パラメータを向上
    change_flag = JPEG_PARAM_UP;
    if(this->yuv_format != TJSAMP_444){
        param_flag = JPEG_YUV_CHANGE;
        this->switchYUV(change_flag);
    }else if(this->quality < JPEG_QUALITY_MAX){
        param_flag = JPEG_QUALITY_CHANGE;
        this->switchQuality(change_flag);
    }
    
    // パラメータの最良値を更新
    this->tuning_mode = DEC_SPEED_UP;
    this->best_quality = this->quality;
    this->best_yuv_format = this->yuv_format;
    
    // 時間計測を再開
    this->wait_t_sum = 0.0;
    this->sync_t_sum = 0.0;
    this->view_t_sum = 0.0; 
}

/* フレーム受信速度を調整 */
void SyncMessageGenerator::ctrlRecvSpeed(int& param_flag, int& change_flag){
    const int recvbuf_stored = this->recv_buf->getStoredNum();
    const int diff_stored = recvbuf_stored - this->pre_recvbuf_stored;
    this->pre_recvbuf_stored = recvbuf_stored;
    
    // バッファ内フレーム数が減ったら一時的にパラメータ低減
    if(recvbuf_stored==0 || diff_stored<-1){
        change_flag = JPEG_PARAM_DOWN;
        if(this->yuv_format != TJSAMP_420){
            param_flag = JPEG_YUV_CHANGE;
            this->switchYUV(change_flag);
        }else if(this->quality > JPEG_QUALITY_MIN){
            param_flag = JPEG_QUALITY_CHANGE;
            this->switchQuality(change_flag);
        }
    }else{
        if(this->yuv_format != this->best_yuv_format){
            param_flag = JPEG_YUV_CHANGE;
            change_flag = JPEG_PARAM_UP;
            this->switchYUV(change_flag);
        }else if(this->quality != this->best_quality){
            param_flag = JPEG_QUALITY_CHANGE;
            change_flag = JPEG_PARAM_UP;
            this->switchQuality(change_flag);
        }
    }
}

/* パラメータ変更要求を決定 */
void SyncMessageGenerator::decideTuningParams(int& param_flag, int& change_flag){
    switch(this->tuning_mode){
    case DEC_SPEED_UP:
        this->speedUpDecode(param_flag, change_flag);
        break;
    case DEC_SPEED_DOWN:
        this->speedDownDecode(param_flag, change_flag);
        break;
    case RECV_SPEED_CTRL:
        this->ctrlRecvSpeed(param_flag, change_flag);
        break;
    }
}

/* 同期メッセージを生成 */
const std::string SyncMessageGenerator::generate(){
    // 変更要求の内容を決定
    int param_flag = JPEG_NO_CHANGE;
    int change_flag = JPEG_PARAM_KEEP;
    ++this->frame_count;
    if(this->frame_count == this->tuning_term){
        this->decideTuningParams(param_flag, change_flag);
        this->frame_count = 0;
    }
    
    // JSON形式でシリアライズ
    this->sync_params.setIntParam("param", param_flag);
    this->sync_params.setIntParam("change", change_flag);
    return this->sync_params.serialize();
}

