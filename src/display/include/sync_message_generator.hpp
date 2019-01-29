/*********************************
*   sync_message_generator.hpp   *
*     (同期メッセージ生成器)     *
*********************************/

#ifndef SYNC_MESSAGE_GENERATOR_HPP
#define SYNC_MESSAGE_GENERATOR_HPP

#include "sync_utils.hpp"
#include "json_handler.hpp"
#include "transceive_framebuffer.hpp"
#include "view_framebuffer.hpp"
extern "C"{
    #include <turbojpeg.h>
}

/* 同期メッセージ生成器 */
class SyncMessageGenerator{
    private:
        JsonHandler sync_params;       // 同期メッセージ記述用のパラメータ
        const int tuning_term;         // パラメータ変更要求の周期
        const tranbuf_ptr_t recv_buf;  // 受信フレームバッファ
        double pre_wait_t = 300.0;     // 前周期での表示バッファ供給待ち時間
        int pre_recvbuf_stored;        // 前周期での受信バッファ利用領域数
        int sampling_type;             // クロマサブサンプル比
        int best_sampling_type;        // 最良のクロマサブサンプル比
        int quality;                   // JPEG品質係数
        int best_quality;              // 最良の品質係数
        int frame_count = 0;           // 表示済フレーム数
        const double available_time;   // 1フレームあたりの猶予時間
        
        void tuneQuality(int& param_flag, int& change_flag, const int type);       // 品質係数を変更
        void tuneSamplingType(int& param_flag, int& change_flag, const int type);  // クロマサブサンプル比を変更
        void decideTuningRequest(int& param_flag, int& change_flag);  // パラメータ変更要求の内容を決定
    
    public:
        double wait_t_sum = 0.0;  // 表示バッファ供給待ち時間
        double sync_t_sum = 0.0;  // 同期処理の所要時間
        double view_t_sum = 0.0;  // 表示処理の所要時間
        bool dec_tuned = false;   // チューニングフェーズ制御用フラグ
        
        SyncMessageGenerator(const int target_fps, const int tuning_term,  // コンストラクタ
                             const tranbuf_ptr_t recv_buf, const int sampling_type,
                             const int quality);
        const std::string generate();                                      // 同期メッセージを生成
};

#endif  /* SYNC_MESSAGE_GENERATOR_HPP */

