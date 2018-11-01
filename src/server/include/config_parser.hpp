/*******************************
*      config_parser.hpp       *
*   (設定ファイルのパーサー)   *
*******************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "print_with_mutex.hpp"
#include <vector>
#include <tuple>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace _pt = boost::property_tree;
using vs_params_t = std::tuple<const char*, int, int>;
using fs_params_t = std::tuple<std::string, int, std::vector<std::string>>;

/* 設定ファイルのパーサー */
class ConfigParser{
    private:
        const char *video_src;             // 再生する動画
        int row;                           // ディスプレイの横の枚数
        int column;                        // ディスプレイの縦の枚数
        int port;                          // フロントエンドサーバ用のポート
        int sender_port;                   // フレーム送信用ポート
        std::string protocol;              // フレーム送信用プロトコル
        std::vector<std::string> ip_list;  // ディスプレイノードのIP
        
        bool setParams(_pt::ptree& conf);  // パラメータを取得
    public:
        ConfigParser(const char* const filename);  // コンストラクタ
        int getFrontendServerParams();             // フロントエンドサーバ用に値を取得
        vs_params_t getVideoSplitterParams();      // フレーム分割器用に値を取得
        fs_params_t getFrameSenderParams();        // フレーム送信器用に値を取得
};

#endif  /* CONFIG_PARSER_HPP */

