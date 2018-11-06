/*******************************
*      config_parser.cpp       *
*   (設定ファイルのパーサー)   *
*******************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const std::string filename):
    BaseConfigParser(filename)
{   
    // パラメータを取得
    if(!this->setParams(this->conf)){
        std::exit(EXIT_FAILURE);
    }
}

/* パラメータを取得 */
bool ConfigParser::setParams(const _pt::ptree& conf){
    try{
        this->ip = conf.get_optional<std::string>("head_node.ip").get();
        this->port = conf.get_optional<int>("head_node.port").get();
        this->res_x = conf.get_optional<int>("resolution.width").get();
        this->res_y = conf.get_optional<int>("resolution.height").get();
        this->width = conf.get_optional<int>("frame.width").get();
        this->height = conf.get_optional<int>("frame.height").get();
    }catch(...){
        print_err("Could not get parameter", "Config file is invalid");
        return false;
    }
    return true;
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

