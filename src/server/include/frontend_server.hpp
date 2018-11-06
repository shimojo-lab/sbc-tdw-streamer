/*****************************
*     frontend_server.hpp    *
*   (フロントエンドサーバ)   *
*****************************/

#ifndef FRONTEND_SERVER_HPP
#define FRONTEND_SERVER_HPP

#include "config_parser.hpp"
#include "video_splitter.hpp"
#include "frame_sender.hpp"

using vs_ptr_t = std::unique_ptr<VideoSplitter>;

/* フロントエンドサーバ */
class FrontendServer{
    private:
        const ios_ptr_t ios;                     // I/Oイベントループ
        _asio::io_service::strand strand;        // I/O排他制御
        _ip::tcp::socket sock;                   // TCPソケット
        acc_ptr_t acc;                           // TCPメッセージ受信器
        std::string protocol;                    // フレーム送信用プロトコル
        vs_ptr_t splitter;                       // フレーム分割器
        int display_num;                         // 総ディスプレイノード数
        int sender_port;                         // フレーム送信用ポート
        int framerate;                           // フレームレート
        int frame_num;                           // 総フレーム数
        int connection_num = 0;                  // ディスプレイノードの接続数
        std::vector<std::string> ip_list;        // ディスプレイノードのIP
        std::vector<boost::thread> thread_list;  // フレーム送信用スレッド
        bar_ptr_t barrier;                       // 同期用バリア
        
        void onConnect(const _sys::error_code& err);  // TCP接続時のコールバック
        void onSendInit(const _sys::error_code& err, std::size_t t_bytes,
                        std::string ip, std::size_t id);  // 初期化メッセージ送信時のコールバック
        void runFrameSender(const fq_ptr_t queue);  // 別スレッドでフレーム送信器を起動
    public:
        FrontendServer(const ios_ptr_t ios, ConfigParser& parser);  // コンストラクタ
};

#endif  /* FRONTEND_SERVER_HPP */

