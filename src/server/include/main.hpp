/********************************
 *           main.hpp           *
 *  (ヘッドノード用送信サーバ)  *
 ********************************/

#ifndef MAIN_HPP
#define MAIN_HPP

#include "config_parser.hpp"
#include "video_splitter.hpp"
#include "frame_sender.hpp"

/* 定数の定義 */
extern const int ARGUMENT_NUM;  // コマンドライン引数の個数

/* 別スレッド実行用の関数 */
void thread_task(smt_ios_t ios, smt_fq_t queue, const char *ip, int port);

#endif  /* MAIN_HPP */

