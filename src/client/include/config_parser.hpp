/*******************************
*      config_parser.hpp       *
*   (設定ファイルのパーサー)   *
*******************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "base_config_parser.hpp"

namespace _pt = boost::property_tree;
using rc_params_t = std::tuple<std::string, int>;
using fv_params_t = std::tuple<int, int, int, int>;

/* 設定ファイルのパーサー */
class ConfigParser : public BaseConfigParser{
    private:
        std::string ip;  // ヘッドノードのIP
        int port;        // ヘッドノードのポート
        int res_x;       // ディスプレイの横の長さ
        int res_y;       // ディスプレイの縦の長さ
        int width;       // フレームの横の長さ
        int height;      // フレームの縦の長さ
    
        bool setParams(const _pt::ptree& conf) override;  // パラメータを取得
    
    public:
        ConfigParser(const std::string filename);  // コンストラクタ
        rc_params_t getRequestClientParams();      // 接続要求クライアント用に値を取得
        fv_params_t getFrameViewerParams();        // フレーム表示器用に値を取得
        int getFrameReceiverParams();              // フレーム受信器用に値を取得
};

#endif  /* CONFIG_PARSER_HPP */

