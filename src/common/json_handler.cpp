/*************************
*    json_handler.cpp    *
*     (JSONハンドラ)     *
*************************/

#include "json_handler.hpp"

/* JSONをシリアライズ */
const std::string JsonHandler::serialize(){
    std::stringstream ss;
    _pt::write_json(ss, this->json, false);
    return ss.str();
}

/* JSONをデシリアライズ */
void JsonHandler::deserialize(const std::string& json_str){
    std::stringstream ss;
    ss << json_str;
    _pt::read_json(ss, this->json);
}

/* JSON内のパラメータを取得 (int用) */
const int JsonHandler::getIntParam(const std::string& key){
    return std::stoi(this->json.get_optional<std::string>(key).get());
}

/* JSON内のパラメータを取得 (文字列用) */
const std::string JsonHandler::getStringParam(const std::string& key){
    return this->json.get_optional<std::string>(key).get();
}

/* JSONにパラメータを追加 (int用) */
void JsonHandler::setIntParam(const std::string& key, const int param){
    this->json.put(key, param);
}

/* JSONにパラメータを追加 (文字列用) */
void JsonHandler::setStringParam(const std::string& key, const std::string& param){
    this->json.put(key, param);
}

