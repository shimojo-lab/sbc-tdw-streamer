/******************************
*   viewer_synchronizer.hpp   *
*        (同期制御器)         *
******************************/

#ifndef VIEWER_SYNCHRONIZER_HPP
#define VIEWER_SYNCHRONIZER_HPP

#include "socket_utils.hpp"
#include "sync_utils.hpp"
#include "print_with_mutex.hpp"
#include <atomic>

using streambuf_ptr_t = std::shared_ptr<_asio::streambuf>;

/* 同期制御器 */
class ViewerSynchronizer{
    private:
        ios_t& ios;                                   // I/Oイベントループ
        std::vector<tcps_ptr_t>& sock_list;           // 接続済TCPソケット
        std::vector<streambuf_ptr_t> streambuf_list;  // TCP受信用バッファ
        const int display_num;                        // 全ディスプレイ数
        std::atomic<int> sync_count;                  // 同期済ディスプレイ数
        std::atomic<int>& comp_quality;               // フレーム圧縮品質
        std::atomic<bool>& send_semaphore;            // 送信制御セマフォ
        
        void parseSync(std::string& recv_msg);                                 // 同期メッセージをパース
        void onRecvSync(const err_t& err, std::size_t t_bytes, const int id);  // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, std::size_t t_bytes, const int id);  // 同期メッセージ送信時のコールバック
        void sendSync();                                                       // 同期メッセージを送信
    
    public:
        ViewerSynchronizer(ios_t& ios, std::vector<tcps_ptr_t>& sock_list, std::atomic<int>& comp_quality, std::atomic<bool>& send_ready);  // コンストラクタ
        void run();  // 同期メッセージの受信を開始
};

#endif  /* VIEWER_SYNCHRONIZER_HPP */

