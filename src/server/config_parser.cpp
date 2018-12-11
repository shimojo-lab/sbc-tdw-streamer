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

/* パラメータを取得するメソッド */
const bool ConfigParser::setParams(const _pt::ptree& conf){
    // 各種パラメータを取得
    try{
        this->video_src = conf.get_optional<std::string>("video_src").get();
        this->row = conf.get_optional<int>("layout.row").get();
        this->column = conf.get_optional<int>("layout.column").get();
        this->frontend_port = conf.get_optional<int>("port.frontend").get();
        this->sender_port = conf.get_optional<int>("port.sender").get();
        this->sbuf_size = conf.get_optional<int>("send_buf_size").get();
        this->init_quality = conf.get_optional<int>("init_image_quality").get();
    }catch(...){
        print_err("Could not get parameter", "Config file is invalid");
        return false;
    }
    
    // ディスプレイノードのIPを取得
    for(const auto& elem : conf.get_child("display_node")){
        this->ip_list.push_back(elem.second.data());
    }
    if(int(this->ip_list.size()) != this->row*this->column){
        print_err("Number of display nodes is invalid", std::to_string(this->ip_list.size()));
        return false;
    }
    
    // フレーム送信用プロトコルを取得
    const std::string protocol = conf.get_optional<std::string>("protocol").get();
    if(protocol == "tcp" || protocol == "TCP"){
        this->protocol_type = TCP_STREAMING_FLAG;
    }else if(protocol == "udp" || protocol == "UDP"){
        this->protocol_type = UDP_STREAMING_FLAG;
    }else{
        print_err("Invaild protocol is selected", protocol);
        return false;
    }
    return true;
}

/* フロントエンドサーバのポート番号を取得 */
const int ConfigParser::getFrontendServerPort(){
    return this->frontend_port;
}

/* フロントエンドサーバへパラメータ渡し */
fs_params_t ConfigParser::getFrontendServerParams(){
    std::string video_src = this->video_src;
    int row = this->row;
    int column = this->column;
    int sender_port = this->sender_port;
    int  protocol_type = this->protocol_type;
    int sbuf_size = this->sbuf_size;
    int init_quality = this->init_quality;
    std::vector<std::string> ip_list = this->ip_list;
    return std::forward_as_tuple(video_src, row, column, sender_port, protocol_type, sbuf_size, init_quality, ip_list);
}

