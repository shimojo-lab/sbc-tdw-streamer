/*******************************
*      config_parser.hpp       *
*   (設定ファイルのパーサー)   *
*******************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "base_config_parser.hpp"
#include <vector>

using fs_params_t = std::tuple<std::string, int, int, int, int, int, int, std::vector<std::string>>;

/* 設定ファイルのパーサー */
class ConfigParser : public BaseConfigParser{
    private:
        std::string video_src;             // 再生動画のソース
        int row;                           // ディスプレイの横の枚数
        int column;                        // ディスプレイの縦の枚数
        int frontend_port;                 // フロントエンドサーバ用のポート
        int sender_port;                   // フレーム送信用ポート
        int protocol_type;                 // フレーム送信用プロトコル
        int sbuf_size;                     // 送信バッファのサイズ
        int init_quality;                  // 初期圧縮品質
        std::vector<std::string> ip_list;  // ディスプレイノードのIP
        
        bool setParams(const _pt::ptree& conf) override;  // パラメータを取得
    
    public:
        ConfigParser(const std::string filename);  // コンストラクタ
        int getFrontendServerPort();               // フロントエンドサーバのポート番号を取得
        fs_params_t getFrontendServerParams();     // フロントエンドサーバへパラメータ渡し
};

#endif  /* CONFIG_PARSER_HPP */

