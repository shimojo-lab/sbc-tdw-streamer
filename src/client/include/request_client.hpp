/*****************************
*     request_client.hpp     *
* (ディスプレイクライアント) *
*****************************/

#ifndef REQUEST_CLIENT_HPP
#define REQUEST_CLIENT_HPP

#include "config_parser.hpp"
#include "tcp_frame_receiver.hpp"
#include "frame_viewer.hpp"
#include <thread>

/* ディスプレイクライアント */
class RequestClient{
    private:
        ios_t& ios;                 // I/Oイベントループ
        tcp_t::socket sock;         // TCPソケット
        _asio::streambuf recv_buf;  // 受信メッセージ用バッファ
        std::string ip;             // ヘッドノードのIP
        int res_x;                  // ディスプレイの横の長さ
        int res_y;                  // ディスプレイの縦の長さ
        int width;                  // フレームの横の長さ
        int height;                 // フレームの縦の長さ
        boost::thread recv_thre;    // フレーム受信スレッド
        
        void onConnect(const err_t& err);                   // TCP接続時のコールバック
        void onRecvInit(const err_t& err, size_t t_bytes);  // 初期化メッセージ受信時のコールバック
        void runFrameReceiver(const fq_ptr_t queue, const std::string protocol, const int port);  // 別スレッドでフレーム受信器を起動
    
    public:
        RequestClient(ios_t& ios, ConfigParser& parser);  // コンストラクタ
};

#endif  /* REQUEST_CLIENT_HPP */

