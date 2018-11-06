/*********************************
*     base_config_parser.cpp     *
* (設定ファイルのパーサーの雛形) *
*********************************/

#include "base_config_parser.hpp"

/* コンストラクタ */
BaseConfigParser::BaseConfigParser(const std::string filename){
    // 設定ファイルをパース
    try{
        _pt::read_json(filename, this->conf);
    }catch(...){
        print_err("Failed to read config file", filename);
        std::exit(EXIT_FAILURE);
    }
}

/* パラメータを設定 */
bool BaseConfigParser::setParams(const _pt::ptree& conf){
    return true;
}

/* パースした設定内容を取得 */
const _pt::ptree& BaseConfigParser::getParsedConfig(){
    return this->conf;
}

