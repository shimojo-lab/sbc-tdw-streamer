/*****************************
*     config_parser.hpp      *
*   (設定ファイルのパーサ)   *
*****************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "socket_utils.hpp"
#include "base_config_parser.hpp"
#include <vector>
extern "C"{
    #include <turbojpeg.h>
}

using fs_params_t = std::tuple<
    std::string, int, int, int, int, int, int, int, int, int, int, int, int, int, int,
    std::vector<std::string>
>;

/* 設定ファイルのパーサ */
class ConfigParser : public BaseConfigParser{
    private:
        std::string video_src;              // 再生動画のソース
        int framerate;                      // 目標フレームレート
        int column;                         // ディスプレイの横の枚数
        int row;                            // ディスプレイの縦の枚数
        int bezel_w;                        // 横のベゼル幅
        int bezel_h;                        // 縦のベゼル幅
        int width;                          // ディスプレイの横の画素数
        int height;                         // ディスプレイの縦の画素数
        int fs_port;                        // フロントエンドサーバ用ポート
        int stream_port;                    // フレーム送信用ポート
        int sendbuf_size;                   // 送信フレームバッファのサイズ
        int recvbuf_size;                   // 受信バッファサイズのサイズ
        int sampling_type;                  // クロマサブサンプル比の初期値
        int quality;                        // JPEG品質係数の初期値
        int dec_thre_num;                   // フレーム展開スレッドの数
        int tuning_term;                    // JPEGパラメータの調整周期
        std::vector<std::string> ip_addrs;  // ディスプレイノードのIP
        
        const bool readParams(const _pt::ptree& conf) override;  // パラメータを読み込み
    
    public:
        ConfigParser(const std::string filename);     // コンストラクタ
        const int getFrontendServerPort();            // フロントエンドサーバのポート番号を取得
        const fs_params_t getFrontendServerParams();  // フロントエンドサーバへ値渡し
};

#endif  /* CONFIG_PARSER_HPP */

