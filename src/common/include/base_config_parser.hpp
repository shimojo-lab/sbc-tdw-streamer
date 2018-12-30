/*************************************
*       base_config_parser.hpp       *
*  (設定ファイルパーサの基底クラス)  *
*************************************/

#ifndef BASE_CONFIG_PARSER_HPP
#define BASE_CONFIG_PARSER_HPP

#include "mutex_logger.hpp"
#include <tuple>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace _pt = boost::property_tree;

/* 設定ファイルパーサの基底クラス */
class BaseConfigParser{
    private:
        virtual const bool readParams(const _pt::ptree& conf);  // パラメータを読み込み
    
    protected:
        _pt::ptree conf;  // パラメータ群
    
    public:
        BaseConfigParser(const std::string filename);  // コンストラクタ
};

#endif  /* BASE_CONFIG_PARSER_HPP */

