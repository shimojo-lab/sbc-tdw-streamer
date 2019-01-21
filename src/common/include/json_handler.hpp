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
        const std::string serialize();                              // JSONをシリアライズ
        void deserialize(const std::string& json_str);              // JSONをデシリアライズ
        const int getIntParam(const std::string& key);              // JSON内のパラメータを取得 (int用)
        const std::string getStringParam(const std::string& key);   // JSON内のパラメータを取得 (文字列用)
        void setIntParam(const std::string& key, const int param);  // JSONにパラメータを追加 (int用)
        void setStringParam(const std::string& key,                 // JSONにパラメータを追加 (文字列用)
                            const std::string& param);
};

#endif  /* JSON_HANDLER_HPP */

