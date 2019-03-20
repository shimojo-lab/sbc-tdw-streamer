/**********************************
*        config_parser.cpp        *
*  (parser of display_conf.json)  *
**********************************/

#include "config_parser.hpp"

/* constructor (load display_conf.json) */
ConfigParser::ConfigParser(const std::string& conf_file):
    BaseConfigParser(conf_file)
{
    if(!this->readParams(this->conf)){
        std::exit(EXIT_FAILURE);
    }
}

/* read the parameters in JSON */
const bool ConfigParser::readParams(const _pt::ptree& conf){
    try{
        this->ip = this->getStrParam("head_node.ip");
        this->port = this->getIntParam("head_node.port");
        this->fb_dev = this->getStrParam("device.framebuffer");
    }catch(...){
        _ml::caution("Could not get parameter", "Config file is invalid");
        return false;
    }
    return true;
}

/* pass the parameters to the display client */
const dc_params_t ConfigParser::getDisplayClientParams(){
    const std::string ip = this->ip;
    const int port = this->port;
    const std::string fb_dev = this->fb_dev;
    return std::forward_as_tuple(ip, port, fb_dev);
}

