/*******************************
*      config_parser.hpp       *
*   (設定ファイルのパーサー)   *
*******************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "base_config_parser.hpp"
#include <vector>

using vs_params_t = std::tuple<std::string, int, int>;
using fs_params_t = std::tuple<std::string, int, std::vector<std::string>>;

/* 設定ファイルのパーサー */
class ConfigParser : public BaseConfigParser{
    private:
        std::string video_src;             // 再生する動画
        int row;                           // ディスプレイの横の枚数
        int column;                        // ディスプレイの縦の枚数
        int frontend_port;                 // フロントエンドサーバ用のポート
        int sender_port;                   // フレーム送信用ポート
        std::string protocol;              // フレーム送信用プロトコル
        std::vector<std::string> ip_list;  // ディスプレイノードのIP
        
        bool setParams(const _pt::ptree& conf) override;  // パラメータを取得
    
    public:
        ConfigParser(const std::string filename);  // コンストラクタ
        int getFrontendServerParams();             // フロントエンドサーバ用に値を取得
        vs_params_t getVideoSplitterParams();      // フレーム分割器用に値を取得
        fs_params_t getFrameSenderParams();        // フレーム送信器用に値を取得
};

#endif  /* CONFIG_PARSER_HPP */

