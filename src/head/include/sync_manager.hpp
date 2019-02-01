/*************************
*    sync_manager.hpp    *
*      (同期制御器)      *
*************************/

#ifndef SYNC_MANAGER_HPP
#define SYNC_MANAGER_HPP

#include "mutex_logger.hpp"
#include "socket_utils.hpp"
#include "sync_utils.hpp"
#include "json_handler.hpp"
#include <cmath>
extern "C"{
    #include <turbojpeg.h>
}

using streambuf_ptr_t = std::shared_ptr<_asio::streambuf>;

/* 同期制御器 */
class SyncManager{
    private:
        _asio::io_service& ios;                    // I/Oイベントループ
        std::vector<sock_ptr_t>& socks;            // 接続済TCPソケット
        std::vector<streambuf_ptr_t> stream_bufs;  // ストリームバッファ
        const int display_num;                     // 全ディスプレイ数
        const int target_fps;                      // 目標フレームレート
        std::atomic_int sync_count;                // 同期済ディスプレイ数
        jpeg_params_t& yuv_format_list;            // YUVサンプル比
        jpeg_params_t& quality_list;               // 品質係数
        JsonHandler sync_params;                   // 同期メッセージのパラメータ変更要求
        hr_clock_t pre_t;                          // 1周期の開始時刻
        int frame_count = 0;                       // 表示済フレーム数
        double elapsed_t = 0.0;                    // 経過時間
        int total_sec = 0;                         // 経過秒数
        
        const std::string tuneYUV(const int change_flag, const int id);      // YUVサンプル比を変更
        const std::string tuneQuality(const int change_flag, const int id);  // 品質係数を変更
        void parseSyncMsg(const std::string& msg, const int id);             // 同期メッセージをパース
        void onRecvSync(const err_t& err, size_t t_bytes, const int id);     // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes, const int id);     // 同期メッセージ送信時のコールバック
        void sendSync();                                                     // 同期メッセージを送信
        
    public:
        SyncManager(_asio::io_service& ios, std::vector<sock_ptr_t>& socks,  // コンストラクタ
                    const int target_fps, jpeg_params_t& yuv_format_list,
                    jpeg_params_t& quality_list);
        void run();  // 同期メッセージの受信を開始
};

#endif  /* SYNC_MANAGER_HPP */

