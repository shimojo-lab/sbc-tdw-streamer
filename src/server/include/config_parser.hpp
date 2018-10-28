/********************************
 *      config_parser.hpp       *
 *   (設定ファイルのパーサー)   *
 ********************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>

namespace _pt = boost::property_tree;
using vs_params_t = std::tuple<const char*, int, int>;
using fs_params_t = std::tuple<std::vector<std::string>, std::vector<int>>;

/* 設定ファイルのパーサー */
class ConfigParser{
    private:
        const char *video_src;             // 再生する動画
        int row;                           // ディスプレイの横の枚数
        int column;                        // ディスプレイの縦の枚数
        int display_num;                   // 全ディスプレイ数
        std::vector<std::string> ip_list;  // 送信先のIP
        std::vector<int> port_list;        // 送信先のポート
    public:
        ConfigParser(const char* const filename);  // コンストラクタ
        ~ConfigParser();                           // デストラクタ
        const int getDisplayNum();                 // ディスプレイ数を取得
        vs_params_t getVideoSplitterParams();      // フレーム分割器用に値を取得
        fs_params_t getFrameSenderParams();        // 分割フレーム送信器用に値を取得
};

#endif  /* CONFIG_PARSER_HPP */

