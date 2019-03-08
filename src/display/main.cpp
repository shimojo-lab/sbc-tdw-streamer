/***************************************
*               main.cpp               *
*  (the program for the display node)  *
***************************************/

#include "main.hpp"

/* the main function */
int main(int argc, char *argv[]){
    // parse display_conf.json
    if(argc != ARGUMENT_NUM){
        _ml::caution("Number of arguments is invalid", "Usage: sbc_client <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // launch the display client
    _asio::io_service ios;
    DisplayClient client(ios, parser);
    ios.run();
    
    return EXIT_SUCCESS;
}

