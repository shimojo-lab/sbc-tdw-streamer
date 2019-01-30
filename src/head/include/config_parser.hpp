/*****************************
*     config_parser.hpp      *
*   (設定ファイルのパーサ)   *
*****************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "socket_utils.hpp"
#include "base_config_parser.hpp"
#include <vector>

using ip_list_t = std::vector<std::string>;
using fs_params_t = std::tuple<
    std::string, int, int, int, int, int, int, int, int, int, int, std::string,
    int, int, int, ip_list_t
>;

/* 設定ファイルのパーサ */
class ConfigParser : public BaseConfigParser{
    private:
        std::string src;         // 再生動画のソース
        int target_fps;          // 目標フレームレート
        int column;              // ディスプレイの横の枚数
        int row;                 // ディスプレイの縦の枚数
        int bezel_w;             // 横のベゼル幅
        int bezel_h;             // 縦のベゼル幅
        int width;               // ディスプレイの横の画素数
        int height;              // ディスプレイの縦の画素数
        int fs_port;             // フロントエンドサーバ用ポート
        int stream_port;         // フレーム送信用ポート
        int sendbuf_num;         // 送信フレームバッファの領域数
        int recvbuf_num;         // 受信フレームバッファの領域数
        std::string yuv_format;  // YUVサンプル比の初期設定
        int quality;             // 品質係数の初期値
        int dec_thre_num;        // フレーム展開スレッド数
        int tuning_term;         // JPEGパラメータのチューニング周期
        ip_list_t ip_addrs;      // ディスプレイノードのIP
        
        const bool readParams(const _pt::ptree& conf) override;  // パラメータを読み込み
    
    public:
        ConfigParser(const std::string& filename);    // コンストラクタ
        const int getFrontendServerPort();            // フロントエンドサーバのポート番号を取得
        const fs_params_t getFrontendServerParams();  // フロントエンドサーバへ値渡し
};

#endif  /* CONFIG_PARSER_HPP */

