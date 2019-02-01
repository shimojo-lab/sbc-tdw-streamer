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
    min_available_t(1000.0/(double)(target_fps+FPS_JITTER)),
    max_available_t(1000.0/(double)(target_fps-FPS_JITTER))
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

/* フレーム展開速度を確認 */
const bool SyncMessageGenerator::checkDecodeSpeed(int& param_flag, int& change_flag){
    // バッファ供給待ち時間を算出
    const double wait_t = this->wait_t_sum / (double)this->tuning_term;
    const double sync_t = this->sync_t_sum / (double)this->tuning_term;
    const double view_t = this->view_t_sum / (double)this->tuning_term;
    const double min_wait_t = this->min_available_t - sync_t - view_t;
    const double max_wait_t = this->max_available_t - sync_t - view_t;
    
    // パラメータ変更内容を決定
    bool result = false;
    if(wait_t > max_wait_t){  // 速度が不十分な場合はパラメータを低減
        result = true;
        change_flag = JPEG_PARAM_DOWN;
        if(this->yuv_format != TJSAMP_420){
            param_flag = JPEG_YUV_CHANGE;
            this->switchYUV(change_flag);
        }else if(this->quality > JPEG_QUALITY_MIN){
            param_flag = JPEG_QUALITY_CHANGE;
            this->switchQuality(change_flag);
        }
    }else if(wait_t < min_wait_t){  // 速度が過剰な場合はパラメータを向上
        result = true;
        change_flag = JPEG_PARAM_UP;
        if(this->quality < JPEG_QUALITY_MAX){
            param_flag = JPEG_QUALITY_CHANGE;
            this->switchQuality(change_flag);
        }else if(this->yuv_format != TJSAMP_444){
            param_flag = JPEG_YUV_CHANGE;
            this->switchYUV(change_flag);
        }
    }else{  // 現在のパラメータを最良値として採用
        this->best_quality = this->quality;
        this->best_yuv_format = this->yuv_format;
        this->pre_recvbuf_stored = this->recv_buf->getStoredNum();
    }
    
    // 時間計測を再開
    this->wait_t_sum = 0.0;
    this->sync_t_sum = 0.0;
    this->view_t_sum = 0.0;
    return result;
}

/* フレーム受信速度を確認 */
void SyncMessageGenerator::checkRecvSpeed(int& param_flag, int& change_flag){
    // 受信フレームバッファ残量の変化を算出
    const int recvbuf_stored = this->recv_buf->getStoredNum();
    const int diff_stored = recvbuf_stored - this->pre_recvbuf_stored;
    this->pre_recvbuf_stored = recvbuf_stored;
    
    // パラメータ変更内容を決定
    if(recvbuf_stored<=4 || diff_stored<-1){  // 受信フレームバッファ内
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

/* 同期メッセージを生成 */
const std::string SyncMessageGenerator::generate(){
    int param_flag = JPEG_NO_CHANGE;
    int change_flag = JPEG_PARAM_KEEP;
    ++this->frame_count;
    
    // 一定周期でパラメータ変更の必要性を判定
    if(this->frame_count == this->tuning_term){
        if(!this->checkDecodeSpeed(param_flag, change_flag)){
            this->checkRecvSpeed(param_flag, change_flag);
        }
        this->frame_count = 0;
    }
    
    // JSON形式でシリアライズ
    this->sync_params.setIntParam("param", param_flag);
    this->sync_params.setIntParam("change", change_flag);
    return this->sync_params.serialize();
}

