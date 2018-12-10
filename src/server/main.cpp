/*******************************
*           main.cpp           *
*  (ヘッドノード側プログラム)  *
*******************************/

#include "main.hpp"

/* Main関数 */
int main(int argc, char *argv[]){
    // 設定ファイルをパース
    if(argc != ARGUMENT_NUM){
        print_err("Number of arguments is invalid", "Usage: sbc_server <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // フロントエンドサーバを起動
    ios_t ios;
    FrontendServer server(ios, parser, parser.getFrontendServerPort());
    ios.run();
    
    return 0;
}

