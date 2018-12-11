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
        ios_t& ios;                                // I/Oイベントループ
        tcp_t::socket sock;                        // TCPソケット
        _asio::streambuf stream_buf;               // TCP受信用バッファ
        std::string ip;                            // ヘッドノードのIP
        int res_x;                                 // ディスプレイの横の長さ
        int res_y;                                 // ディスプレイの縦の長さ
        msgbuf_ptr_t rbuf;                         // 受信フレームバッファ
        framebuf_ptr_t vbuf;                       // 表示フレームバッファ
        boost::thread recv_thre;                   // フレーム受信スレッド
        std::vector<boost::thread> dec_thre_list;  // フレーム展開スレッド
        
        void onConnect(const err_t& err);                   // ヘッドノード接続時のコールバック
        void onRecvInit(const err_t& err, size_t t_bytes);  // 初期化メッセージ受信時のコールバック
        void runFrameDecoder(const int id, const int row, const int column, const int width, const int height);  // 別スレッドでフレーム展開器を起動
        void runFrameReceiver(const int port, const int protocol_type);  // 別スレッドでフレーム受信器を起動
    
    public:
        DisplayClient(ios_t& ios, ConfigParser& parser);  // コンストラクタ
};

#endif  /* DISPLAY_CLIENT_HPP */

