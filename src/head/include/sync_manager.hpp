/*************************
*    sync_manager.hpp    *
*      (同期制御器)      *
*************************/

#ifndef SYNC_MANAGER_HPP
#define SYNC_MANAGER_HPP

#include "socket_utils.hpp"
#include "sync_utils.hpp"
#include "mutex_logger.hpp"
#include <atomic>

using streambuf_ptr_t = std::shared_ptr<_asio::streambuf>;

/* 同期制御器 */
class SyncManager{
    private:
        _asio::io_service& ios;                    // I/Oイベントループ
        std::vector<sock_ptr_t>& socks;            // 接続済TCPソケット
        std::vector<streambuf_ptr_t> stream_bufs;  // ストリームバッファ
        const int display_num;                     // 全ディスプレイ数
        std::atomic<int> sync_count;               // 同期済ディスプレイ数
        std::atomic<int>& sampling_type;           // クロマサブサンプリングの形式
        std::atomic<int>& quality;                 // 量子化品質係数
        
        void parseSync(std::string& recv_msg);                            // 同期メッセージをパース
        void onRecvSync(const err_t& err, size_t t_bytes, const int id);  // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes, const int id);  // 同期メッセージ送信時のコールバック
        void sendSync();                                                  // 同期メッセージを送信
        
    public:
        SyncManager(_asio::io_service& ios, std::vector<sock_ptr_t>& socks,  // コンストラクタ
                    std::atomic<int>& sampling_type, std::atomic<int>& quality);
        void run();  // 同期メッセージの受信を開始
};

#endif  /* SYNC_MANAGER_HPP */

