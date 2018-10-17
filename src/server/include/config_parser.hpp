/********************************
 *      config_parser.hpp       *
 *   (設定読み込みモジュール)   *
 ********************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

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
        const char *filename;              // 再生する動画ファイル
        int row;                           // ディスプレイの横の長さ
        int column;                        // ディスプレイの縦の長さ
        std::vector<std::string> ip_list;  // ディスプレイノードのIP
        std::vector<int> port_list;        // ディスプレイノードのポート
        int display_num;                   // 全ディスプレイ数
    public:
        ConfigParser(const char *conf_file);         // コンストラクタ
        ~ConfigParser();                             // デストラクタ
        void setHeadNodeParams(_pt::ptree conf);     // ヘッドノードのパラメータを設定
        void setDisplayNodeParams(_pt::ptree conf);  // ディスプレイノードのパラメータを設定
        int getDisplayNum();                         // ディスプレイ数を取得
        std::tuple<const char*, int, int> getVideoDemuxerParams();    // フレーム分割モジュール用に値を取得
        std::tuple<const char*, int> getFrameStreamerParams(int id);  // フレーム送信モジュール用に値を取得
};

#endif

