/************************************
*       base_config_parser.hpp      *
*  (super class of config parsers)  *
************************************/

#ifndef BASE_CONFIG_PARSER_HPP
#define BASE_CONFIG_PARSER_HPP

#include "mutex_logger.hpp"
#include <tuple>
#include <cstdlib>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace _pt = boost::property_tree;

/* a super class of config parsers */
class BaseConfigParser{
    private:
        virtual const bool readParams(const _pt::ptree& conf);  // read the parameters
    
    protected:
        _pt::ptree conf;  // a parsed json
        
        const int getIntParam(const std::string& key);          // get an int parameter
        const double getDoubleParam(const std::string& key);    // get a double parameter
        const std::string getStrParam(const std::string& key);  // get a string parameter
    
    public:
        BaseConfigParser(const std::string& filename);  // constructor
};

#endif  /* BASE_CONFIG_PARSER_HPP */

