/*****************************
*     config_parser.cpp      *
*   (設定ファイルのパーサ)   *
*****************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const std::string conf_file):
    BaseConfigParser(conf_file)
{
    // パラメータを取得
    if(!this->readParams(this->conf)){
        std::exit(EXIT_FAILURE);
    }
}

/* パラメータを読み込み */
const bool ConfigParser::readParams(const _pt::ptree& conf){
    // 各種パラメータを取得
    try{
        this->video_src = conf.get_optional<std::string>("video_src").get();
        this->column = conf.get_optional<int>("layout.column").get();
        this->row = conf.get_optional<int>("layout.row").get();
        this->width = conf.get_optional<int>("resolution.width").get();
        this->height = conf.get_optional<int>("resolution.height").get();
        this->fs_port = conf.get_optional<int>("port.frontend_server").get();
        this->stream_port = conf.get_optional<int>("port.frame_streamer").get();
        this->sendbuf_size = conf.get_optional<int>("compression.buffer_size").get();
        this->quality = conf.get_optional<int>("compression.init_quality").get();
    }catch(...){
        print_err("Could not get parameter", "Config file is invalid");
        return false;
    }
    
    // ディスプレイノードのIPを取得
    for(const auto& elem : conf.get_child("display_node")){
        this->ip_addrs.push_back(elem.second.data());
    }
    if(int(this->ip_addrs.size()) != this->row*this->column){
        print_err("Number of display nodes is invalid", std::to_string(this->ip_addrs.size()));
        return false;
    }
    return true;
}

/* フロントエンドサーバのポート番号を取得 */
const int ConfigParser::getFrontendServerPort(){
    return this->fs_port;
}

/* フロントエンドサーバへ値渡し */
const fs_params_t ConfigParser::getFrontendServerParams(){
    const std::string video_src = this->video_src;
    const int row = this->row;
    const int column = this->column;
    const int width = this->width;
    const int height = this->height;
    const int stream_port = this->stream_port;
    const int sendbuf_size = this->sendbuf_size;
    const int quality = this->quality;
    std::vector<std::string> ip_addrs = this->ip_addrs;
    return std::forward_as_tuple(video_src, row, column, width, height, stream_port,
                                 sendbuf_size, quality, ip_addrs);
}

