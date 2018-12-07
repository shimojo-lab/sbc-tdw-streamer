/*******************************
*      config_parser.hpp       *
*   (設定ファイルのパーサー)   *
*******************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "base_config_parser.hpp"

using dc_params_t = std::tuple<std::string, int, int, int, int, int>;

/* 設定ファイルのパーサー */
class ConfigParser : public BaseConfigParser{
    private:
        std::string ip;  // ヘッドノードのIP
        int port;        // ヘッドノードのフロントエンドポート
        int res_x;       // ディスプレイの横の長さ
        int res_y;       // ディスプレイの縦の長さ
        int rbuf_size;   // 受信バッファのサイズ
        int vbuf_size;   // 表示バッファのサイズ
        
        bool setParams(const _pt::ptree& conf) override;  // パラメータを取得
        
    public:
        ConfigParser(const std::string filename);  // コンストラクタ
        dc_params_t getDisplayClientParams();      // ディスプレイクライアントへパラメータ渡し
};

#endif  /* CONFIG_PARSER_HPP */

