/********************************
 *      config_parser.cpp       *
 *   (設定ファイルのパーサー)   *
 ********************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const char *conf_file){
    // 設定ファイルをパース
    _pt::ptree conf;
    _pt::read_json(conf_file, conf);
    
    // パラメータを取得
    this->video_src = conf.get_optional<std::string>("video_src").get().c_str();
    this->row = conf.get_optional<int>("layout.row").get();
    this->column = conf.get_optional<int>("layout.column").get();
    this->display_num = this->row * this->column;
    
    // ディスプレイノードを登録
    int count = 0;
    BOOST_FOREACH(_pt::ptree::value_type &child, conf.get_child("display_node")){
        const _pt::ptree &node = child.second;
        this->ip_list.push_back(node.get_optional<std::string>("ip").get());
        this->port_list.push_back(node.get_optional<int>("port").get());
        ++count;
    }
    if(count != this->display_num){
        std::cerr << "[Error] Number of display nodes is invalid." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/* デストラクタ */
ConfigParser::~ConfigParser(){}

/* 全ディスプレイ数を取得 */
int ConfigParser::getDisplayNum(){
    return this->display_num;
}

/* フレーム分割器用に値を取得 */
std::tuple<const char*, int, int> ConfigParser::getVideoSplitterParams(){
    const char *video_src = this->video_src;
    int row = this->row;
    int column = this->column;
    return std::forward_as_tuple(video_src, row, column);
}

/* 分割フレーム送信器用に値を取得 */
std::tuple<const char*, int> ConfigParser::getFrameSenderParams(int id){
    const char *ip = this->ip_list[id].c_str();
    int port = this->port_list[id];
    return std::forward_as_tuple(ip, port);
}

