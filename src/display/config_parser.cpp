/*****************************
*     config_parser.cpp      *
*   (設定ファイルのパーサ)   *
*****************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const std::string& conf_file):
    BaseConfigParser(conf_file)
{   
    // パラメータを読み込み
    if(!this->readParams(this->conf)){
        std::exit(EXIT_FAILURE);
    }
}

/* パラメータを読み込み */
const bool ConfigParser::readParams(const _pt::ptree& conf){
    try{
        this->ip = this->getStrParam("head_node.ip");
        this->port = this->getIntParam("head_node.port");
        this->fb_dev = this->getStrParam("device.framebuffer");
        this->tty_dev = this->getStrParam("device.tty");
    }catch(...){
        _ml::caution("Could not get parameter", "Config file is invalid");
        return false;
    }
    return true;
}

/* ディスプレイクライアントへ値渡し */
const dc_params_t ConfigParser::getDisplayClientParams(){
    const std::string ip = this->ip;
    const int port = this->port;
    const std::string fb_dev = this->fb_dev;
    const std::string tty_dev = this->tty_dev;
    return std::forward_as_tuple(ip, port, fb_dev, tty_dev);
}

