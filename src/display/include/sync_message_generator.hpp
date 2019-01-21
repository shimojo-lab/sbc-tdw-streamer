/*********************************
*   sync_message_generator.hpp   *
*     (同期メッセージ生成器)     *
*********************************/

#ifndef SYNC_MESSAGE_GENERATOR_HPP
#define SYNC_MESSAGE_GENERATOR_HPP

#include "sync_utils.hpp"
#include "json_handler.hpp"
#include "transceive_framebuffer.hpp"
#include <chrono>

/* 同期メッセージ生成器 */
class SyncMessageGenerator{
    private:
        JsonHandler sync_params;       // 同期メッセージに記述するパラメータ
        const int target_fps;          // 目標フレームレート
        const int tuning_term;         // パラメータ変更要求の周期
        const tranbuf_ptr_t recv_buf;  // 受信フレームバッファ
        int frame_count = 0;           // 表示済フレーム数
        hr_chrono_t start_time;        // 周期の開始時刻
        int pre_recvbuf_stored;        // 周期開始時点の受信フレームバッファ初期使用領域数
        
        void decideTuningRequest(int& param_flag, int& change_flag);  // パラメータ変更要求の内容を決定
    
    public:
        SyncMessageGenerator(const int target_fps, const int tuning_term,  // コンストラクタ
                             const tranbuf_ptr_t recv_buf);
        const std::string generate();                                      // 同期メッセージを生成
};

#endif  /* SYNC_MESSAGE_GENERATOR_HPP */

