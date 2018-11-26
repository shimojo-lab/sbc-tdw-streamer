/*******************************
*    viewer_synchronizer.hpp   *
*    (表示タイミング制御器)    *
*******************************/

#ifndef VIEWER_SYNCHRONIZER_HPP
#define VIEWER_SYNCHRONIZER_HPP

#include "boost_socket.hpp"
#include "print_with_mutex.hpp"
#include <atomic>
#include <boost/thread/barrier.hpp>

using buf_ptr_t = std::unique_ptr<_asio::streambuf>;
using bar_ptr_t = std::shared_ptr<boost::barrier>;

/* 表示タイミング制御器 */
class ViewerSynchronizer{
    private:
        ios_t& ios;                          // I/Oイベントループ
        ios_t::strand strand;                // I/O排他制御
        std::vector<tcps_ptr_t>& sock_list;  // 接続済TCPソケット
        std::vector<buf_ptr_t> buf_list;     // 受信用バッファ
        const int display_num;               // 全ディスプレイ数
        std::atomic<int> sync_count;         // 同期済ディスプレイ数
        
        void onRecvSync(const err_t& err, size_t t_bytes, const int id);  // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes, const int id);  // 同期メッセージ送信時のコールバック
        void sendSync();  // 同期メッセージを送信
    
    public:
        ViewerSynchronizer(ios_t& ios, std::vector<tcps_ptr_t>& sock_list);  // コンストラクタ
        void run();  // 同期メッセージの受信を開始
};

#endif  /* VIEWER_SYNCHRONIZER_HPP */

