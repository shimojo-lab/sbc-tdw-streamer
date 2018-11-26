/*******************************
*    viewer_synchronizer.hpp   *
*    (表示タイミング制御器)    *
*******************************/

#ifndef VIEWER_SYNCHRONIZER_HPP
#define VIEWER_SYNCHRONIZER_HPP

#include "boost_socket.hpp"
#include "print_with_mutex.hpp"
#include <boost/thread/barrier.hpp>

using bar_ptr_t = std::shared_ptr<boost::barrier>;

/* 表示タイミング制御器 */
class ViewerSynchronizer{
    private:
        ios_t& ios;                           // I/Oイベントループ
        std::vector<tcps_ptr_t>& sock_list;   // 接続済TCPソケット
        tcp_t::acceptor& acc;                 // TCPメッセージ受信器
        _asio::streambuf recv_buf;            // 受信用バッファ
        int display_num;                      // 全ディスプレイ数
        bar_ptr_t barrier;                    // 同期用バリア
        
        void onRecvSync(const err_t& err, size_t t_bytes, const int i);  // 同期メッセージ受信時のコールバック
        void onSendSync(const err_t& err, size_t t_bytes);  // 同期メッセージ送信時のコールバック
    
    public:
        ViewerSynchronizer(ios_t& ios, std::vector<tcps_ptr_t>& sock_list, tcp_t::acceptor& acc);  // コンストラクタ
        void run();  // 同期メッセージの受信を開始
};

#endif  /* VIEWER_SYNCHRONIZER_HPP */

