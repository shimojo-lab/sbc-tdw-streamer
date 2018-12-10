/*****************************
*     frontend_server.hpp    *
*   (フロントエンドサーバ)   *
*****************************/

#ifndef FRONTEND_SERVER_HPP
#define FRONTEND_SERVER_HPP

#include "config_parser.hpp"
#include "frame_compresser.hpp"
#include "tcp_frame_sender.hpp"
#include "udp_frame_sender.hpp"
#include "viewer_synchronizer.hpp"

/* フロントエンドサーバ */
class FrontendServer{
    private:
        ios_t& ios;                         // I/Oイベントループ
        tcps_ptr_t sock;                    // TCPソケット
        tcp_t::acceptor acc;                // TCPアクセプタ
        std::vector<tcps_ptr_t> sock_list;  // 接続済TCPソケット
        std::string video_src;              // 再生動画のソース
        int row;                            // ディスプレイの横の枚数
        int column;                         // ディスプレイの縦の枚数
        int width;                          // フレームの幅
        int height;                         // フレームの高さ
        int sender_port;                    // フレーム送信用ポート
        int protocol_type;                  // フレーム送信用プロトコル
        int connection_num = 0;             // ディスプレイノードの接続数
        std::atomic<int> comp_quality;      // 圧縮品質
        std::vector<std::string> ip_list;   // ディスプレイノードのIP
        msgbuf_ptr_t sbuf;                  // 送信バッファ
        boost::thread sender_thre;          // フレーム送信スレッド
        boost::thread compresser_thre;      // フレーム圧縮スレッド
        int id = 0;
        void onConnect(const err_t& err);  // TCP接続時のコールバック
        void onSendInit(const err_t& err, size_t t_bytes, const std::string ip);  // 初期化メッセージ送信時のコールバック
        void runTCPFrameSender();      // 別スレッドでTCP版フレーム送信器を起動
        void runUDPFrameSender();      // 別スレッドでUDP版フレーム送信器を起動
        void runFrameCompresser();     // 別スレッドでフレーム圧縮器を起動
        void runViewerSynchronizer();  // 同スレッドで同期制御器を起動
    
    public:
        FrontendServer(ios_t& ios, ConfigParser& parser, const int port);  // コンストラクタ
};

#endif  /* FRONTEND_SERVER_HPP */

