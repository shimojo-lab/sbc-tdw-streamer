/*************************************
*             main.cpp               *
*  (ディスプレイノード側プログラム)  *
*************************************/

#include "main.hpp"

const int ARGUMENT_NUM = 2;    // コマンドライン引数の個数
const int ARGUMENT_INDEX = 1;  // コマンドライン引数のインデックス

/* Main関数 */
int main(int argc, char* argv[]){
    // 設定ファイルをパース
    if(argc != ARGUMENT_NUM){
        print_err("Number of arguments is invalid", "Usage: sbc_client <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // 接続要求クライアントを起動
    ios_ptr_t ios = std::make_shared<_asio::io_service>();
    RequestClient client(ios, parser);
    ios->run();
    return 0;
}

