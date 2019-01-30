/*******************************
*           main.cpp           *
*  (ヘッドノード用プログラム)  *
*******************************/

#include "main.hpp"

/* Main関数 */
int main(int argc, char *argv[]){
    // 設定ファイルをパース
    std::string conf_file;
    if(argc != ARGUMENT_NUM){
        _ml::caution("Number of arguments is invalid", "Usage: head_server <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // フロントエンドサーバを起動
    _asio::io_service ios;
    FrontendServer server(ios, parser, parser.getFrontendServerPort());
    ios.run();
    
    return EXIT_SUCCESS;
}

