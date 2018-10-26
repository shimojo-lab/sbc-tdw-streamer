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

/* 名前空間のエイリアス */
namespace _pt = boost::property_tree;

/* 設定ファイルのパーサー */
class ConfigParser{
    private:
        const char *video_src;             // 再生する動画
        int row;                           // ディスプレイの横の枚数
        int column;                        // ディスプレイの縦の枚数
        int display_num;                   // 全ディスプレイ数
        std::vector<std::string> ip_list;  // ディスプレイノードのIP
        std::vector<int> port_list;        // ディスプレイノードのポート
    public:
        ConfigParser(const char *conf_file);  // コンストラクタ
        ~ConfigParser();                      // デストラクタ
        int getDisplayNum();                  // ディスプレイ数を取得
        std::tuple<const char*, int, int> getVideoSplitterParams();  // フレーム分割器用に値を取得
        std::tuple<const char*, int> getFrameSenderParams(int id);   // 分割フレーム送信器用に値を取得
};

#endif  /* CONFIG_PARSER_HPP */

