/*****************************
*     frontend_server.hpp    *
*   (フロントエンドサーバ)   *
*****************************/

#ifndef FRONTEND_SERVER_HPP
#define FRONTEND_SERVER_HPP

#include "config_parser.hpp"
#include "video_splitter.hpp"
#include "tcp_frame_sender.hpp"
#include "viewer_synchronizer.hpp"

using vs_ptr_t = std::shared_ptr<VideoSplitter>;

/* フロントエンドサーバ */
class FrontendServer{
    private:
int id = 0;
        ios_t& ios;                              // I/Oイベントループ
        tcps_ptr_t sock;                         // TCPソケット
        tcp_t::acceptor acc;                     // TCPメッセージ受信器
        std::vector<tcps_ptr_t> sock_list;       // 接続済TCPソケット
        std::string protocol;                    // フレーム送信用プロトコル
        vs_ptr_t splitter;                       // フレーム分割器
        int display_num;                         // 総ディスプレイノード数
        int sender_port;                         // フレーム送信用ポート
        int frame_num;                           // 総フレーム数
        double frame_rate;                       // フレームレート
        int connection_num = 0;                  // ディスプレイノードの接続数
        std::vector<std::string> ip_list;        // ディスプレイノードのIP
        std::vector<boost::thread> sender_thre;  // フレーム送信スレッド
        boost::thread splitter_thre;             // フレーム分割スレッド
        
        void onConnect(const err_t& err);  // TCP接続時のコールバック
        void onSendInit(const err_t& err, size_t t_bytes, const std::string ip);  // 初期化メッセージ送信時のコールバック
        void runFrameSender(const fq_ptr_t queue);       // 別スレッドでフレーム送信器を起動
        void runVideoSplitter(const vs_ptr_t splitter);  // 別スレッドでフレーム分割器を起動
        void runViewerSynchronizer();                    // 表示タイミング制御器を起動
    
    public:
        FrontendServer(ios_t& ios, ConfigParser& parser);  // コンストラクタ
};

#endif  /* FRONTEND_SERVER_HPP */

