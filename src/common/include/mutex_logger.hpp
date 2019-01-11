/*****************************
*      mutex_logger.hpp      *
*    (排他制御付きロガー)    *
*****************************/

#ifndef MUTEX_LOGGER_HPP
#define MUTEX_LOGGER_HPP

#include <iostream>
#include <string>
#include <mutex>

const std::string GREEN("\033[1;32m");   // 緑文字出力
const std::string YELLOW("\033[1;33m");  // 黄文字出力
const std::string RED("\033[0;31m");     // 赤文字出力
const std::string CYAN("\033[0;36m");    // シアン文字出力
const std::string RESET("\033[0m");      // 文字色リセット

/* 排他制御付きロガー */
namespace mutex_logger{
    std::unique_lock<std::mutex> log_lock();                         // 排他制御を実行
    void notice(const std::string& msg);                             // 通知メッセージを表示
    void warn(const std::string& msg, const std::string& suppl);     // 警告メッセージを表示
    void caution(const std::string& msg, const std::string& suppl);  // エラーメッセージを表示
    void debug(const std::string& msg);                              // String型変数を表示
    void debug(const int num);                                       // int型変数を表示
    void debug(const double num);                                    // double型変数を表示
}

namespace _ml = mutex_logger;

#endif  /* MUTEX_LOGGER_HPP */

