/********************************
 *      config_parser.hpp       *
 *   (設定ファイルのパーサー)   *
 ********************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <tuple>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

/* 名前空間のエイリアス */
namespace _pt = boost::property_tree;

/* 設定ファイルのパーサー */
class ConfigParser{
    private:
        int res_x;   // ディスプレイの横の長さ
        int res_y;   // ディスプレイの縦の長さ
        int width;   // フレームの横の長さ
        int height;  // フレームの縦の長さ
        int port;    // 受信用のポート
    public:
        ConfigParser(const char *conf_file);                    // コンストラクタ
        ~ConfigParser();                                        // デストラクタ
        std::tuple<int, int, int, int> getFrameViewerParams();  // フレーム表示器用に値を取得
        int getFrameReceiverParams();                           // フレーム受信器用に値を取得
};

#endif  /* CONFIG_PARSER_HPP */

