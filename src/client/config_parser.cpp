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
const bool ConfigParser::setParams(const _pt::ptree& conf){
    try{
        this->ip = conf.get_optional<std::string>("head_node.ip").get();
        this->port = conf.get_optional<int>("head_node.port").get();
        this->res_x = conf.get_optional<int>("resolution.width").get();
        this->res_y = conf.get_optional<int>("resolution.height").get();
        this->rbuf_size = conf.get_optional<int>("recv_buf_size").get();
        this->vbuf_size = conf.get_optional<int>("view_buf_size").get();
    }catch(...){
        print_err("Could not get parameter", "Config file is invalid");
        return false;
    }
    return true;
}

/* ディスプレイクライアントへパラメータ渡し */
dc_params_t ConfigParser::getDisplayClientParams(){
    std::string ip = this->ip;
    int port = this->port;
    int res_x = this->res_x;
    int res_y = this->res_y;
    int rbuf_size = this->rbuf_size;
    int vbuf_size = this->vbuf_size;
    return std::forward_as_tuple(ip, port, res_x, res_y, rbuf_size, vbuf_size);
}

