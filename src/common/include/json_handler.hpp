/*************************
*    json_handler.hpp    *
*     (JSONハンドラ)     *
*************************/

#ifndef JSON_HANDLER_HPP
#define JSON_HANDLER_HPP

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace _pt = boost::property_tree;

/* JSONハンドラ */
class JsonHandler{
    private:
        _pt::ptree json;  // JSON本体
    
    public:
        const std::string serialize();                           // JSONをシリアライズ
        void deserialize(const std::string& json_str);           // JSONをデシリアライズ
        const int getParam(const std::string& key);              // JSON内のパラメータを取得
        void setParam(const std::string& key, const int param);  // JSONにパラメータを追加
};

#endif  /* JSON_HANDLER_HPP */

