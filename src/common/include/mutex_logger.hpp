/*********************************
*        mutex_logger.hpp        *
*  (排他制御付きロギング用関数)  *
*********************************/

#ifndef MUTEX_LOGGER_HPP
#define MUTEX_LOGGER_HPP

#include <iostream>
#include <string>
#include <mutex>

std::unique_lock<std::mutex> console_lock();                        // 排他制御を実行
void print_info(const std::string& msg);                            // 一般メッセージを表示
void print_warn(const std::string& msg, const std::string& suppl);  // 警告メッセージを表示
void print_err(const std::string& msg, const std::string& suppl);   // エラーメッセージを表示
void print_debug(const std::string& msg);                           // デバッグメッセージを表示 (文字列用)
void print_debug(const int& msg);                                   // デバッグメッセージを表示 (int型用)
void print_debug(const double& msg);                                // デバッグメッセージを表示 (double型用)

#endif  /* MUTEX_LOGGER_HPP */

