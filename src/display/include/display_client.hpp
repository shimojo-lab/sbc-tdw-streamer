/*******************************
*     display_client.hpp       *
*  (ディスプレイクライアント)  *
*******************************/

#ifndef DISPLAY_CLIENT_HPP
#define DISPLAY_CLIENT_HPP

#include "config_parser.hpp"
#include "viewer_framebuffer.hpp"
#include "frame_receiver.hpp"
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
        std::string fb_dev;                    // フレームバッファのデバイスファイル
        std::string tty_dev;                   // 端末デバイスのデバイスファイル
        boost::thread recv_thre;               // フレーム受信スレッド
        std::vector<boost::thread> dec_thres;  // フレーム展開スレッド
        
        void onConnect(const err_t& err);                   // ヘッドノード接続時のコールバック
        void onRecvInit(const err_t& err, size_t t_bytes);  // 初期化メッセージ受信時のコールバック
        void runFrameReceiver(const int stream_port,        // 別スレッドでフレーム受信器を起動
                              const jpegbuf_ptr_t recv_buf);
        void runFrameDecoder(const jpegbuf_ptr_t recv_buf,  // 別スレッドでフレーム展開器を起動
                             const rawbuf_ptr_t view_buf);
    
    public:
        DisplayClient(_asio::io_service& ios, ConfigParser& parser);  // コンストラクタ
};

#endif  /* DISPLAY_CLIENT_HPP */

