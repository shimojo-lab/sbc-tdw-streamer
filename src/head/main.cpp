/**************************************
*              main.cpp               *
*  (main function for the head node)  *
**************************************/

#include "main.hpp"

/* main function */
int main(int argc, char *argv[]){
    // parse the head_conf.json
    std::string conf_file;
    if(argc != ARGUMENT_NUM){
        _ml::caution("Number of arguments is invalid", "Usage: head_server <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // launch the frontend server
    _asio::io_service ios;
    FrontendServer server(ios, parser, parser.getFrontendServerPort());
    ios.run();
    
    return EXIT_SUCCESS;
}

