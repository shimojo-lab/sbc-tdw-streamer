/************************************
*       base_config_parser.cpp      *
*  (super class of config parsers)  *
************************************/

#include "base_config_parser.hpp"

/* constructor (parse a json file) */
BaseConfigParser::BaseConfigParser(const std::string& conf_file){
    try{
        _pt::read_json(conf_file, this->conf);
    }catch(...){
        _ml::caution("Failed to read config file", conf_file);
        std::exit(EXIT_FAILURE);
    }
}

/* read the parameters in the config file */
const bool BaseConfigParser::readParams(const _pt::ptree& conf){
    return true;
}

/* get an int parameter */
const int BaseConfigParser::getIntParam(const std::string& key){
    const int param = this->conf.get_optional<int>(key).get();
    return param;
}

/* get a double parameter */
const double BaseConfigParser::getDoubleParam(const std::string& key){
    const double param = this->conf.get_optional<double>(key).get();
    return param;
}

/* get a string parameter */
const std::string BaseConfigParser::getStrParam(const std::string& key){
    const std::string param = this->conf.get_optional<std::string>(key).get();
    return param;
}

