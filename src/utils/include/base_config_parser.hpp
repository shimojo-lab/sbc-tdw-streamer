/***************************************
*        base_config_parser.hpp        *
* (設定ファイルのパーサーの基底クラス) *
***************************************/

#ifndef BASE_CONFIG_PARSER_HPP
#define BASE_CONFIG_PARSER_HPP

#include "print_with_mutex.hpp"
#include <tuple>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace _pt = boost::property_tree;

/* 設定ファイルのパーサーの基底クラス */
class BaseConfigParser{
    private:
        virtual const bool setParams(const _pt::ptree& conf);  // パラメータを取得
    
    protected:
        _pt::ptree conf;  // 設定内容
    
    public:
        BaseConfigParser(const std::string filename);  // コンストラクタ
};

#endif  /* BASE_CONFIG_PARSER_HPP */

