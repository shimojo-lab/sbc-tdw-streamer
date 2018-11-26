/*******************************
*           main.cpp           *
*  (ヘッドノード側プログラム)  *
*******************************/

#include "main.hpp"

/* 定数の定義 */
const int ARGUMENT_NUM = 2;                    // コマンドライン引数の個数
const int ARGUMENT_INDEX = 1;                  // コマンドライン引数のインデックス
const std::string SEPARATOR = "\r\n";          // 受信メッセージのセパレータ
const int SEPARATOR_LEN = SEPARATOR.length();  // 受信メッセージのセパレータの長さ

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
    FrontendServer server(ios, parser);
    ios.run();
    return 0;
}

