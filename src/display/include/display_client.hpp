/*******************************
*     display_client.hpp       *
*  (ディスプレイクライアント)  *
*******************************/

#ifndef DISPLAY_CLIENT_HPP
#define DISPLAY_CLIENT_HPP

#include "config_parser.hpp"
#include "tcp_frame_receiver.hpp"
#include "udp_frame_receiver.hpp"
#include "frame_decoder.hpp"
#include "frame_viewer.hpp"
#include <boost/thread.hpp>

/* ディスプレイクライアント */
class DisplayClient{
    private:
        _asio::io_service& ios;                // I/Oイベントループ
        _ip::tcp::socket sock;                 // TCPソケット
        _asio::streambuf stream_buf;           // ストリームバッファ
        std::string ip;                        // ヘッドノードのIP
        msgbuf_ptr_t rbuf;                     // 受信フレームバッファ
        framebuf_ptr_t vbuf;                   // 表示フレームバッファ
        std::string fb_dev;                    // フレームバッファのデバイスファイル
        boost::thread recv_thre;               // フレーム受信スレッド
        std::vector<boost::thread> dec_thres;  // フレーム展開スレッド
        
        void onConnect(const err_t& err);                   // ヘッドノード接続時のコールバック
        void onRecvInit(const err_t& err, size_t t_bytes);  // 初期化メッセージ受信時のコールバック
        void runFrameDecoder();                             // 別スレッドでフレーム展開器を起動
        void runFrameReceiver(const int port);              // 別スレッドでフレーム受信器を起動
    
    public:
        DisplayClient(_asio::io_service& ios, ConfigParser& parser);  // コンストラクタ
};

#endif  /* DISPLAY_CLIENT_HPP */

