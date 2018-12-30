/*************************************
*             main.cpp               *
*  (ディスプレイノード側プログラム)  *
*************************************/

#include "main.hpp"

/* Main関数 */
int main(int argc, char *argv[]){
    // 設定ファイルをパース
    if(argc != ARGUMENT_NUM){
        print_err("Number of arguments is invalid", "Usage: sbc_client <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // ディスプレイクライアントを起動
    _asio::io_service ios;
    DisplayClient client(ios, parser);
    ios.run();
    
    return 0;
}

