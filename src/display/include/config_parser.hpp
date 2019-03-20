/**********************************
*        config_parser.hpp        *
*  (parser of display_conf.json)  *
**********************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "base_config_parser.hpp"

using dc_params_t = std::tuple<std::string, int, std::string>;

/* parser of display_conf.json */
class ConfigParser : public BaseConfigParser{
    private:
        std::string ip;      // the IP address of the head node
        int port;            // the port number of the head node
        std::string fb_dev;  // the device file of fbdev
        
        const bool readParams(const _pt::ptree& conf) override;  // read the parameters
        
    public:
        ConfigParser(const std::string& conf_file);  // constructor
        const dc_params_t getDisplayClientParams();  // pass the parameters to the display client
};

#endif  /* CONFIG_PARSER_HPP */

