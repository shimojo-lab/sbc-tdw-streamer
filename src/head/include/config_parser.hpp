/***********************************
*        config_parser.hpp         *
*  (the parser of head_conf.json)  *
***********************************/

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "socket_utils.hpp"
#include "base_config_parser.hpp"
#include <vector>

using ip_list_t = std::vector<std::string>;
using fs_params_t = std::tuple<
    std::string, int, int, int, int, int, int, int, int, int, int, std::string,
    int, int, int, ip_list_t
>;

/* the parser of head_conf.json */
class ConfigParser : public BaseConfigParser{
    private:
        std::string src;           // the video source
        int target_fps;            // the target frame rate
        int column;                // the number of displays in a horizontal direction
        int row;                   // the number of displays in a vertical direction
        int bezel_w;               // the length of each bezel in a horizontal direction
        int bezel_h;               // the height of each bezel in a vertical direction
        int width;                 // the number of horizontal pixels in each display
        int height;                // the numver of vertical pixels in each display
        int fs_port;               // the port number for the frontend server
        int stream_port;           // the port number for streaming JPEG frames
        int sendbuf_num;           // the number of domains in the send framebuffer
        int recvbuf_num;           // the number of domains in the receive framebuffer
        std::string ycbcr_format;  // the initial value of the YCbCr format
        int quality;               // the initial value of the quality factor
        int dec_thre_num;          // the number of the decoder threads
        int tuning_term;           // the tuning term of the JPEG parameters
        ip_list_t ip_addrs;        // the IP addresses of the display nodes
        
        const bool readParams(const _pt::ptree& conf) override;  // read the parameters
    
    public:
        ConfigParser(const std::string& filename);    // constructor
        const int getFrontendServerPort();            // get the port number for the frontend server
        const fs_params_t getFrontendServerParams();  // pass the parameters to the frontend server
};

#endif  /* CONFIG_PARSER_HPP */

