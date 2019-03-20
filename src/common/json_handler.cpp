/*****************************
*      json_handler.cpp      *
*   (JSON message handler)   *
*****************************/

#include "json_handler.hpp"

/* serialize JSON */
const std::string JsonHandler::serialize(){
    std::stringstream ss;
    _pt::write_json(ss, this->json, false);
    return ss.str();
}

/* desrialize JSON */
void JsonHandler::deserialize(const std::string& json_str){
    std::stringstream ss;
    ss << json_str;
    _pt::read_json(ss, this->json);
}

/* get an int parameter from JSON */
const int JsonHandler::getIntParam(const std::string& key){
    return std::stoi(this->json.get_optional<std::string>(key).get());
}

/* get a double parameter from JSON */
const int JsonHandler::getDoubleParam(const std::string& key){
    return std::stod(this->json.get_optional<std::string>(key).get());
}

/* get a string parameter from JSON */
const std::string JsonHandler::getStringParam(const std::string& key){
    return this->json.get_optional<std::string>(key).get();
}

/* set an int parameter in JSON */
void JsonHandler::setIntParam(const std::string& key, const int param){
    this->json.put(key, param);
}

/* set a double parameter in JSON */
void JsonHandler::setDoubleParam(const std::string& key, const double param){
    this->json.put(key, param);
}

/* set a string parameter in JSON */
void JsonHandler::setStringParam(const std::string& key, const std::string& param){
    this->json.put(key, param);
}

