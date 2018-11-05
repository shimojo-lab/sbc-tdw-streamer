/*******************************
*           main.cpp           *
*  (ヘッドノード側プログラム)  *
*******************************/

#include "main.hpp"

const int ARGUMENT_NUM = 2;    // コマンドライン引数の個数
const int ARGUMENT_INDEX = 1;  // コマンドライン引数のインデックス

/* Main関数 */
int main(int argc, char *argv[]){
    // 設定ファイルをパース
    if(argc != ARGUMENT_NUM){
        print_err("Number of arguments is invalid", "Usage: sbc_server <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // フロントエンドサーバを起動
    ios_ptr_t ios = std::make_shared<_asio::io_service>();
    FrontendServer server(ios, parser);
    ios->run();
    return 0;
}

