/********************************
 *       config_parser.cpp      *
 *   (設定読み込みモジュール)   *
 *******************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const char *conf_file){
    // 設定ファイルをパース
    _pt::ptree conf;
    _pt::read_json(conf_file, conf);
    
    // パラメータを取得
    this->res_width = conf.get_optional<int>("resolution.width").get();
    this->res_height = conf.get_optional<int>("resolution.height").get();
    this->port = conf.get_optional<int>("port").get();
}

/* デストラクタ */
ConfigParser::~ConfigParser(){}

/* フレーム表示モジュール用のゲッター */
std::tuple<int, int> ConfigParser::getFrameViewerParams(){
    int res_width = this->res_width;
    int res_height = this->res_height;
    return std::forward_as_tuple(res_width, res_height);
}

/* フレーム受信モジュール用のゲッター */
int ConfigParser::getFrameReceiverParams(){
    return this->port;
}

