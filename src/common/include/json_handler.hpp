/*****************************
*      json_handler.hpp      *
*   (JSON message handler)   *
*****************************/

#ifndef JSON_HANDLER_HPP
#define JSON_HANDLER_HPP

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace _pt = boost::property_tree;

/* JSON message handler */
class JsonHandler{
    private:
        _pt::ptree json;  // the parsed JSON message
    
    public:
        const std::string serialize();                             // serialize JSON
        void deserialize(const std::string& json_str);             // deserialize JSON
        const int getIntParam(const std::string& key);             // get an int parameter from JSON
        const int getDoubleParam(const std::string& key);          // get a double parameter from JSON
        const std::string getStringParam(const std::string& key);  // get a string parameter from JSON
        void setIntParam(const std::string& key, const int param);              // set an int parameter in JSON
        void setDoubleParam(const std::string& key, const double param);        // set a double parameter in JSON
        void setStringParam(const std::string& key, const std::string& param);  // set a string parameter in JSON
};

#endif  /* JSON_HANDLER_HPP */

