/******************************************
 *                main.hpp                *
 * (ディスプレイノード側表示クライアント) *
 ******************************************/

#ifndef MAIN_HPP
#define MAIN_HPP

#include "config_parser.hpp"
#include "frame_receiver.hpp"
#include "frame_viewer.hpp"

extern const int ARGUMENT_NUM;      // コマンドライン引数の個数
extern const char* const APP_NAME;  // プログラム名

/* 別スレッド実行用の関数 */
void thread_task(const smt_ios_t ios, const smt_fq_t queue, const int port);

#endif  /* MAIN_HPP */

