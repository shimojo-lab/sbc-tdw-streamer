/************************************
*          json_handler.hpp         *
*  (the handler of a JSON message)  *
************************************/

#ifndef JSON_HANDLER_HPP
#define JSON_HANDLER_HPP

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace _pt = boost::property_tree;

/* the handler of a JSON message */
class JsonHandler{
    private:
        _pt::ptree json;  // a parsed JSON
    
    public:
        const std::string serialize();                              // serialize JSON
        void deserialize(const std::string& json_str);              // deserialize JSON
        const int getIntParam(const std::string& key);              // get a int parameter from JSON
        const std::string getStringParam(const std::string& key);   // get a string parameter from JSON
        void setIntParam(const std::string& key, const int param);  // set a int parameter in JSON
        void setStringParam(const std::string& key,                 // set a string parameter in JSON
                            const std::string& param);
};

#endif  /* JSON_HANDLER_HPP */

