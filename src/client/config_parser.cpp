/********************************
 *      config_parser.cpp       *
 *   (設定ファイルのパーサー)   *
 ********************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const char* const filename){
    // 設定ファイルをパース
    _pt::ptree conf;
    try{
        _pt::read_json(filename, conf);
    }catch(...){
        std::cerr << "[Error] Read config failed." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    // パラメータを取得
    this->res_x = conf.get_optional<int>("resolution.width").get();
    this->res_y = conf.get_optional<int>("resolution.height").get();
    this->width = conf.get_optional<int>("frame.width").get();
    this->height = conf.get_optional<int>("frame.height").get();
    this->port = conf.get_optional<int>("port").get();
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

