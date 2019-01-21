/*********************************
*   sync_message_generator.cpp   *
*     (同期メッセージ生成器)     *
*********************************/

#include "sync_message_generator.hpp"

/* コンストラクタ */
SyncMessageGenerator::SyncMessageGenerator(const int target_fps, const int tuning_term, const tranbuf_ptr_t recv_buf):
    target_fps(target_fps),
    tuning_term(tuning_term),
    recv_buf(recv_buf)
{
    // 計測値を初期化
    this->start_time = std::chrono::high_resolution_clock::now();
    this->pre_recvbuf_stored = this->recv_buf->getStoredNum();
}

/* パラメータ変更要求の内容を決定 */
void SyncMessageGenerator::decideTuningRequest(int& param_flag, int& change_flag){
    // 1周期の平均フレームレートを計算
    hr_chrono_t end_time = std::chrono::high_resolution_clock::now();
    const double delta_t = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-this->start_time).count();
    const double fps = 1000.0 / delta_t / (double)this->tuning_term;
    
    // 受信フレームバッファ使用領域数の変化を計算
    const int post_recvbuf_stored = this->recv_buf->getStoredNum();
    const int diff_stored = post_recvbuf_stored - this->pre_recvbuf_stored;
    
    // フレーム展開速度が不十分ならクロマサブサンプル比を変更
    if(fps < target_fps){
        param_flag = JPEG_SAMPLING_CHANGE;
        change_flag = JPEG_PARAM_DOWN;
    }else{
        // フレーム受信速度が不十分なら品質係数を変更
        param_flag = JPEG_QUALITY_CHANGE;
        change_flag = diff_stored<0 ? JPEG_PARAM_DOWN : JPEG_PARAM_UP;
    }
    
    // 計測値を更新
    this->start_time = end_time;
    this->pre_recvbuf_stored = post_recvbuf_stored;
}

/* 同期メッセージを生成 */
const std::string SyncMessageGenerator::generate(){
    // 変更要求の内容を決定
    int param_flag = JPEG_NO_CHANGE;
    int change_flag = JPEG_PARAM_KEEP;
    if(this->frame_count == this->tuning_term){
        this->decideTuningRequest(param_flag, change_flag);
    }
    ++this->frame_count;
    
    // JSON形式でシリアライズ
    this->sync_params.setIntParam("param", param_flag);
    this->sync_params.setIntParam("change", change_flag);
    return this->sync_params.serialize();
}

