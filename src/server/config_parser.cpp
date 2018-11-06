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
    if(!this->setParams(this->getParsedConfig())){
        std::exit(EXIT_FAILURE);
    }
}

/* パラメータを取得するメソッド */
bool ConfigParser::setParams(const _pt::ptree& conf){
    this->video_src = conf.get_optional<std::string>("video_src").get();
    this->row = conf.get_optional<int>("layout.row").get();
    this->column = conf.get_optional<int>("layout.column").get();
    this->frontend_port = conf.get_optional<int>("port.frontend").get();
    this->sender_port = conf.get_optional<int>("port.sender").get();
   
    // ディスプレイノードのIPを取得
    int count = 0;
    for(const auto& elem : conf.get_child("display_node")){
        this->ip_list.push_back(elem.second.data());
        ++count;
    }
    if(count != this->row*this->column){
        print_err("Number of display nodes is invalid", std::to_string(count));
        return false;
    }
    
    // フレーム送信用プロトコルを取得
    std::string protocol = conf.get_optional<std::string>("protocol").get();
    if(protocol == "tcp" || protocol == "TCP"){
        this->protocol = "TCP";
    }else if(protocol == "udp" || protocol == "UDP"){
        this->protocol = "UDP";
    }else{
        print_err("Invaild protocol is selected", protocol);
        return false;
    }
    return true;
}

/* フロントエンドサーバ用に値を取得 */
int ConfigParser::getFrontendServerParams(){
    return this->frontend_port;
}

/* フレーム分割器用に値を取得 */
vs_params_t ConfigParser::getVideoSplitterParams(){
    std::string video_src = this->video_src;
    int row = this->row;
    int column = this->column;
    return std::forward_as_tuple(video_src, row, column);
}

/* フレーム送信器用に値を取得 */
fs_params_t ConfigParser::getFrameSenderParams(){
    std::string protocol = this->protocol;
    int sender_port = this->sender_port;
    std::vector<std::string> ip_list = this->ip_list;
    return std::forward_as_tuple(protocol, sender_port, ip_list);
}

