/*******************************
*           main.cpp           *
*  (ヘッドノード側プログラム)  *
*******************************/

#include "main.hpp"

/* 定数の定義 */
const int ARGUMENT_NUM = 2;                            // コマンドライン引数の個数
const int ARGUMENT_INDEX = 1;                          // コマンドライン引数のインデックス
const std::string MSG_DELIMITER = "--boundary\r\n";    // 送受信メッセージのデリミタ
const std::string SEQ_DELIMITER = "--sequence=";       // シーケンス番号のデリミタ
const int MSG_DEIMITER_LEN = MSG_DELIMITER.length();   // 送受信メッセージのデリミタ長
const int SEQ_DELIMITER_LEN = SEQ_DELIMITER.length();  // シーケンス番号のデリミタ長

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

