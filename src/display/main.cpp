/*************************************
*             main.cpp               *
*  (ディスプレイノード側プログラム)  *
*************************************/

#include "main.hpp"

/* Main関数 */
int main(int argc, char *argv[]){
    // 標準出力の同期をオフ
    std::ios::sync_with_stdio(false);
    
    // 設定ファイルをパース
    if(argc != ARGUMENT_NUM){
        _ml::caution("Number of arguments is invalid", "Usage: sbc_client <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // ディスプレイクライアントを起動
    _asio::io_service ios;
    DisplayClient client(ios, parser);
    client.run();
    
    return EXIT_SUCCESS;
}

