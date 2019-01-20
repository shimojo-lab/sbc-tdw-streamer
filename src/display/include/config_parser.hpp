/*****************************
*     config_parser.hpp      *
*   (設定ファイルのパーサ)   *
*****************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "base_config_parser.hpp"

using dc_params_t = std::tuple<
    std::string, int, std::string, std::string
>;

/* 設定ファイルのパーサ */
class ConfigParser : public BaseConfigParser{
    private:
        std::string ip;       // ヘッドノードのIP
        int port;             // ヘッドノードのポート
        std::string fb_dev;   // フレームバッファのデバイスファイル
        std::string tty_dev;  // 端末デバイスのデバイスファイル
        
        const bool readParams(const _pt::ptree& conf) override;  // パラメータを読み込み
        
    public:
        ConfigParser(const std::string& conf_file);  // コンストラクタ
        const dc_params_t getDisplayClientParams();  // ディスプレイクライアントへ値渡し
};

#endif  /* CONFIG_PARSER_HPP */

