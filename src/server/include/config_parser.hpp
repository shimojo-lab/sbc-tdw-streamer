/********************************
 *      config_parser.hpp       *
 *   (設定読み込みモジュール)   *
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

/* 設定読み込みモジュール */
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
        std::tuple<const char*, int, int> getVideoDemuxerParams();    // フレーム分割モジュール用のゲッター
        std::tuple<const char*, int> getFrameStreamerParams(int id);  // フレーム送信モジュール用のゲッター
};

#endif

