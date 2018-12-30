/*****************************
*     config_parser.hpp      *
*   (設定ファイルのパーサ)   *
*****************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "socket_utils.hpp"
#include "base_config_parser.hpp"
#include <vector>

using fs_params_t = std::tuple<
    std::string, int, int, int, int, int, int, int, std::vector<std::string>
>;

/* 設定ファイルのパーサ */
class ConfigParser : public BaseConfigParser{
    private:
        std::string video_src;              // 再生動画のソース
        int row;                            // ディスプレイの横の枚数
        int column;                         // ディスプレイの縦の枚数
        int width;                          // ディスプレイの横の画素数
        int height;                         // ディスプレイの縦の画素数
        int fs_port;                        // フロントエンドサーバ用ポート
        int stream_port;                    // フレーム送信用ポート
        int sendbuf_size;                   // 送信フレームバッファのサイズ
        int quality;                        // JPEG圧縮品質の初期値
        std::vector<std::string> ip_addrs;  // ディスプレイノードのIP
        
        const bool readParams(const _pt::ptree& conf) override;  // パラメータを読み込み
    
    public:
        ConfigParser(const std::string filename);     // コンストラクタ
        const int getFrontendServerPort();            // フロントエンドサーバのポート番号を取得
        const fs_params_t getFrontendServerParams();  // フロントエンドサーバへ値渡し
};

#endif  /* CONFIG_PARSER_HPP */

