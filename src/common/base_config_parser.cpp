/*************************************
*       base_config_parser.cpp       *
*  (設定ファイルパーサの基底クラス)  *
*************************************/

#include "base_config_parser.hpp"

/* コンストラクタ */
BaseConfigParser::BaseConfigParser(const std::string conf_file){
    // 設定ファイルをパース
    try{
        _pt::read_json(conf_file, this->conf);
    }catch(...){
        print_err("Failed to read config file", conf_file);
        std::exit(EXIT_FAILURE);
    }
}

/* パラメータを設定 */
const bool BaseConfigParser::readParams(const _pt::ptree& conf){
    return true;
}

