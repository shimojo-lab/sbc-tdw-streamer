/********************************
 *           main.hpp           *
 *  (ヘッドノード用送信サーバ)  *
 ********************************/

#ifndef MAIN_HPP
#define MAIN_HPP

#include "config_parser.hpp"
#include "video_splitter.hpp"
#include "frame_sender.hpp"

extern const int ARGUMENT_NUM;  // コマンドライン引数の個数

/* 別スレッド実行用の関数 */
void thread_task(const smt_ios_t ios, const smt_fq_t queue, const char* const ip, const int port, const int protocol, smt_barrier_t barrier);

#endif  /* MAIN_HPP */

