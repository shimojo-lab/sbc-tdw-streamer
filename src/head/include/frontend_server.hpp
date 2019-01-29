/*****************************
*     frontend_server.hpp    *
*   (フロントエンドサーバ)   *
*****************************/

#ifndef FRONTEND_SERVER_HPP
#define FRONTEND_SERVER_HPP

#include "config_parser.hpp"
#include "frame_encoder.hpp"
#include "frame_sender.hpp"
#include "sync_manager.hpp"
#include <boost/thread.hpp>

/* フロントエンドサーバ */
class FrontendServer{
    private:
        _asio::io_service& ios;                // I/Oイベントループ
        sock_ptr_t sock;                       // TCPソケット
        _ip::tcp::acceptor acc;                // TCPアクセプタ
        std::vector<sock_ptr_t> socks;         // 接続済TCPソケット
        int display_num;                       // 全ディスプレイ数
        JsonHandler init_params;               // 初期化メッセージ用パラメータ
        int connected_num = 0;                 // 接続済ディスプレイノード数
        jpeg_params_t sampling_type_list;      // クロマサブサンプル比
        jpeg_params_t quality_list;            // JPEG品質係数
        std::vector<std::string> ip_addrs;     // ディスプレイノードのIP
        std::vector<tranbuf_ptr_t> send_bufs;  // 送信フレームバッファ
        boost::thread send_thre;               // フレーム送信スレッド
        boost::thread enc_thre;                // フレーム圧縮スレッド
        
        void waitForConnection();          // 接続待機を開始
        void onConnect(const err_t& err);  // ディスプレイノード接続時のコールバック
        void onSendInit(const err_t& err,  // 初期化メッセージ送信時のコールバック
                        size_t t_bytes, const std::string ip);
        void runFrameEncoder(const std::string video_src,  // 別スレッドでフレーム符号化器を起動
                             const int column, const int row, const int bezel_w, const int bezel_h,
                             const int width, const int height);
        void runFrameSender(const int stream_port,         // 別スレッドでフレーム送信器を起動
                            const int viewbuf_num);
        void runSyncManager();             // 同スレッドで同期制御器を起動
    
    public:
        FrontendServer(_asio::io_service& ios, ConfigParser& parser, const int fs_port);  // コンストラクタ
};

#endif  /* FRONTEND_SERVER_HPP */

