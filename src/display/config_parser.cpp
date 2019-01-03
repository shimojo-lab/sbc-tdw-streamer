/*****************************
*     config_parser.cpp      *
*   (設定ファイルのパーサ)   *
*****************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const std::string conf_file):
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
        this->ip = conf.get_optional<std::string>("head_node.ip").get();
        this->port = conf.get_optional<int>("head_node.port").get();
        this->fb_dev = conf.get_optional<std::string>("framebuffer_device").get();
        this->rbuf_size = conf.get_optional<int>("recv_buffer_size").get();
        this->vbuf_size = conf.get_optional<int>("view_buffer_size").get();
        this->dec_thre_num = conf.get_optional<int>("decode_threads").get();
    }catch(...){
        print_err("Could not get parameter", "Config file is invalid");
        return false;
    }
    return true;
}

/* ディスプレイクライアントへ値渡し */
const dc_params_t ConfigParser::getDisplayClientParams(){
    std::string ip = this->ip;
    int port = this->port;
    std::string fb_dev = this->fb_dev;
    int rbuf_size = this->rbuf_size;
    int vbuf_size = this->vbuf_size;
    int dec_thre_num = this->dec_thre_num;
    return std::forward_as_tuple(ip, port, fb_dev, rbuf_size, vbuf_size, dec_thre_num);
}

