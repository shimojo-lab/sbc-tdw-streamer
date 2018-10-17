/********************************
 *      config_parser.hpp       *
 *   (設定読み込みモジュール)   *
 ********************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <tuple>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

/* 名前空間のエイリアス */
namespace _pt = boost::property_tree;

/* 設定読み込みモジュール */
class ConfigParser{
    private:
        int res_width;   // ディスプレイの横の長さ
        int res_height;  // ディスプレイの縦の長さ
        int port;        // 受信用のポート
    public:
        ConfigParser(const char *conf_file);          // コンストラクタ
        ~ConfigParser();                              // デストラクタ
        std::tuple<int, int> getFrameViewerParams();  // フレーム表示モジュール用に値を取得
        int getFrameReceiverParams();                 // フレーム受信モジュール用に値を取得
};

#endif

