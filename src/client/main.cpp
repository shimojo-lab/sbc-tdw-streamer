/*************************************
*             main.cpp               *
*  (ディスプレイノード側プログラム)  *
*************************************/

#include "main.hpp"

const int ARGUMENT_NUM = 2;                            // コマンドライン引数の個数
const int ARGUMENT_INDEX = 1;                          // コマンドライン引数のインデックス
const std::string MSG_DELIMITER = "--boundary\r\n";    // 送受信メッセージのデリミタ
const std::string SEQ_DELIMITER = "--sequence=";       // シーケンス番号のデリミタ
const int MSG_DELIMITER_LEN = MSG_DELIMITER.length();  // 送受信メッセージのデリミタ長
const int SEQ_DELIMITER_LEN = SEQ_DELIMITER.length();  // 送受信メッセージのデリミタ長

/* Main関数 */
int main(int argc, char* argv[]){
    // 設定ファイルをパース
    if(argc != ARGUMENT_NUM){
        print_err("Number of arguments is invalid", "Usage: sbc_client <config file>");
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[ARGUMENT_INDEX]);
    
    // ディスプレイクライアントを起動
    ios_t ios;
    DisplayClient client(ios, parser);
    ios.run();
    
    return 0;
}

