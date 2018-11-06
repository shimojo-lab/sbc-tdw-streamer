/*****************************
*     request_client.hpp     *
*   (接続要求クライアント)   *
*****************************/

#ifndef REQUEST_CLIENT_HPP
#define REQUEST_CLIENT_HPP

#include "config_parser.hpp"
#include "tcp_frame_receiver.hpp"
#include "frame_viewer.hpp"

/* 分割フレーム受信器 */
class RequestClient{
    private:
        _ios& ios;                  // I/Oイベントループ
        _ios new_ios;               // 新規I/Oイベントループ
        _tcp::socket sock;          // TCPソケット
        _asio::streambuf recv_buf;  // 受信メッセージ用バッファ
        std::string ip;             // ヘッドノードのIP
        int res_x;                  // ディスプレイの横の長さ
        int res_y;                  // ディスプレイの縦の長さ
        int width;                  // フレームの横の長さ
        int height;                 // フレームの縦の長さ
        
        void onConnect(const _err& err);                   // TCP接続時のコールバック
        void onRecvInit(const _err& err, size_t t_bytes);  // 初期化メッセージ受信時のコールバック
        void runFrameReceiver(_ios::strand& strand, const tcps_ptr_t sock, const fq_ptr_t queue, const int port, const std::string protocol);  // 別スレッドでフレーム受信器を起動
    
    public:
        RequestClient(_ios& ios, ConfigParser& parser);  // コンストラクタ
};

#endif  /* REQUEST_CLIENT_HPP */

