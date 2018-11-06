/*****************************
*     request_client.hpp     *
*   (接続要求クライアント)   *
*****************************/

#ifndef REQUEST_CLIENT_HPP
#define REQUEST_CLIENT_HPP

#include "config_parser.hpp"
#include "frame_receiver.hpp"
#include "frame_viewer.hpp"

/* 分割フレーム受信器 */
class RequestClient{
    private:
        const ios_ptr_t ios;        // I/Oイベントループ
        _asio::io_service::strand strand;  // I/O排他制御
        _ip::tcp::socket sock;      // TCPソケット
        _asio::streambuf recv_buf;  // 受信メッセージ用バッファ
        std::string ip;             // ヘッドノードのIP
        int res_x;                  // ディスプレイの横の長さ
        int res_y;                  // ディスプレイの縦の長さ
        int width;                  // フレームの横の長さ
        int height;                 // フレームの縦の長さ
        
        void onConnect(const _sys::error_code& err);                   // TCP接続時のコールバック
        void onRecvInit(const _sys::error_code& err, size_t t_bytes);  // 初期化メッセージ受信時のコールバック
        void runFrameReceiver(const tcps_ptr_t tcp_sock, const fq_ptr_t queue, const int port, const std::string protocol);  // 別スレッドでフレーム受信器を起動
    public:
        RequestClient(const ios_ptr_t ios, ConfigParser& parser);  // コンストラクタ
};

#endif  /* FRAME_RECEIVER_HPP */

