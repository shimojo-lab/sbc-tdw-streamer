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

namespace _pt = boost::property_tree;
using fv_params_t = std::tuple<int, int, int, int>;

/* 設定ファイルのパーサー */
class ConfigParser{
    private:
        int res_x;   // ディスプレイの横の長さ
        int res_y;   // ディスプレイの縦の長さ
        int width;   // フレームの横の長さ
        int height;  // フレームの縦の長さ
        int port;    // 受信用のポート
    public:
        ConfigParser(const char* const filename);  // コンストラクタ
        fv_params_t getFrameViewerParams();        // フレーム表示器用に値を取得
        int getFrameReceiverParams();              // フレーム受信器用に値を取得
};

#endif  /* CONFIG_PARSER_HPP */

