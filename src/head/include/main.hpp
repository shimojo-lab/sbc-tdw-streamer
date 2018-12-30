/*******************************
*           main.hpp           *
*  (ヘッドノード側プログラム)  *
*******************************/

#ifndef MAIN_HPP
#define MAIN_HPP

#include "frontend_server.hpp"

const int ARGUMENT_NUM = 2;     // コマンドライン引数の個数
const int ARGUMENT_INDEX = 1;   // コマンドライン引数のインデックス
const int PATH_NAME_LEN = 255;  // カレントディレクトリ取得用文字列の長さ
const std::string DEF_CONF_PATH = "/../conf/head_conf.json";  // デフォルトの設定ファイルのパス

#endif  /* MAIN_HPP */

