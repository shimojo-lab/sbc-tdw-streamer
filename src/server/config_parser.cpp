/********************************
 *       config_parser.cpp      *
 *   (設定読み込みモジュール)   *
 *******************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const char *conf_file){
    // 設定ファイルをパース
    _pt::ptree conf;
    _pt::read_json(conf_file, conf);
    
    // パラメータを取得
    this->setHeadNodeParams(conf.get_child("head_node"));
    this->setDisplayNodeParams(conf.get_child("display_node"));
}

/* デストラクタ */
ConfigParser::~ConfigParser(){}

/* ヘッドノードのパラメータを設定するメソッド */
void ConfigParser::setHeadNodeParams(_pt::ptree conf){
    this->filename = conf.get_optional<std::string>("filename").get().c_str();
    this->row = conf.get_optional<int>("layout.row").get();
    this->column = conf.get_optional<int>("layout.column").get();
    this->display_num = this->row * this->column;
    return;
}

/* ディスプレイノードのパラメータを設定するメソッド */
void ConfigParser::setDisplayNodeParams(_pt::ptree conf){
    BOOST_FOREACH(_pt::ptree::value_type &child, conf){
        const _pt::ptree &entry = child.second;
        this->ip_list.push_back(entry.get_optional<std::string>("ip").get());
        this->port_list.push_back(entry.get_optional<int>("port").get());
    }
    return;
}

/* 全ディスプレイ数を取得するメソッド */
int ConfigParser::getDisplayNum(){
    return this->display_num;
}

/* フレーム分割モジュール用のパラメータを取得するメソッド */
std::tuple<const char*, int, int> ConfigParser::getVideoDemuxerParams(){
    const char *filename = this->filename;
    int row = this->row;
    int column = this->column;
    return std::forward_as_tuple(filename, row, column);
}

/* フレーム送信モジュール用のパラメータを取得するメソッド */
std::tuple<const char*, int> ConfigParser::getFrameStreamerParams(int id){
    const char *ip = this->ip_list[id].c_str();
    int port = this->port_list[id];
    return std::forward_as_tuple(ip, port);
}

