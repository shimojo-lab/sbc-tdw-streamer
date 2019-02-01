/*********************************
*   sync_message_generator.hpp   *
*     (同期メッセージ生成器)     *
*********************************/

#ifndef SYNC_MESSAGE_GENERATOR_HPP
#define SYNC_MESSAGE_GENERATOR_HPP

#include "json_handler.hpp"
#include "transceive_framebuffer.hpp"
#include "sync_utils.hpp"
extern "C"{
    #include <turbojpeg.h>
}

const int FPS_JITTER = 1;

/* 同期メッセージ生成器 */
class SyncMessageGenerator{
    private:
        JsonHandler sync_params;       // 同期メッセージ記述用のパラメータ
        const int tuning_term;         // パラメータ変更要求の周期
        const tranbuf_ptr_t recv_buf;  // 受信フレームバッファ
        int pre_recvbuf_stored;        // 前周期での受信フレームバッファ残量
        int yuv_format;                // YUVサンプル比
        int best_yuv_format;           // YUVサンプル比の最良値
        int quality;                   // 品質係数
        int best_quality;              // 品質係数の最良値
        int frame_count = 0;           // 表示済フレーム数
        const double min_available_t;  // 表示フレームバッファ供給待ちの最小猶予時間
        const double max_available_t;  // 表示フレームバッファ供給待ちの最大猶予時間
        
        void switchYUV(const int change_flag);                           // YUVサンプル比を変更
        void switchQuality(const int change_flag);                       // 品質係数を変更
        const bool checkDecodeSpeed(int& param_flag, int& change_flag);  // フレーム展開速度を確認
        void checkRecvSpeed(int& param_flag, int& change_flag);          // フレーム受信速度を確認
    
    public:
        double wait_t_sum = 0.0;         // 表示フレームバッファ供給待ち時間
        double sync_t_sum = 0.0;         // 同期処理の所要時間
        double view_t_sum = 0.0;         // 表示処理の所要時間
        
        SyncMessageGenerator(const int target_fps, const int tuning_term,  // コンストラクタ
                             const tranbuf_ptr_t recv_buf, const int yuv_format,
                             const int quality);
        const std::string generate();                                      // 同期メッセージを生成
};

#endif  /* SYNC_MESSAGE_GENERATOR_HPP */

