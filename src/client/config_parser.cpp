/*******************************
*      config_parser.cpp       *
*   (設定ファイルのパーサー)   *
*******************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const char* const filename){
    // 設定ファイルをパース
    _pt::ptree conf;
    try{
        _pt::read_json(filename, conf);
    }catch(...){
        print_err("Failed to read config", std::string(filename));
        std::exit(EXIT_FAILURE);
    }
    
    // パラメータを取得
    setParams(conf);
}

/* パラメータを取得 */
void ConfigParser::setParams(_pt::ptree& conf){
    this->ip = conf.get_optional<std::string>("head_node.ip").get();
    this->port = conf.get_optional<int>("head_node.port").get();
    this->res_x = conf.get_optional<int>("resolution.width").get();
    this->res_y = conf.get_optional<int>("resolution.height").get();
    this->width = conf.get_optional<int>("frame.width").get();
    this->height = conf.get_optional<int>("frame.height").get();
    return;
}

/* 接続要求クライアント用に値を取得 */
rc_params_t ConfigParser::getRequestClientParams(){
    std::string ip = this->ip;
    int port = this->port;
    return std::forward_as_tuple(ip, port);
}

/* フレーム表示器用に値を取得 */
fv_params_t ConfigParser::getFrameViewerParams(){
    int res_x = this->res_x;
    int res_y = this->res_y;
    int width = this->width;
    int height = this->height;
    return std::forward_as_tuple(res_x, res_y, width, height);
}

/* フレーム受信器用に値を取得 */
int ConfigParser::getFrameReceiverParams(){
    return this->port;
}

