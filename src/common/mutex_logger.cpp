/*****************************
*      mutex_logger.cpp      *
*    (排他制御付きロガー)    *
*****************************/

#include "mutex_logger.hpp"

/* 通知メッセージを表示 */
void mutex_logger::notice(const std::string& msg){
    std::lock_guard<std::mutex> log_lock(mutex_logger::lock);
    std::cout << GREEN
              << "[Info] "
              << RESET
              << msg
              << "."
              << std::endl;
}

/* 警告メッセージを表示 */
void mutex_logger::warn(const std::string& msg, const std::string& suppl){
    std::lock_guard<std::mutex> log_lock(mutex_logger::lock);
    std::cout << YELLOW
              << "[Warning] "
              << RESET
              << msg
              << ". ("
              << suppl
              << ")"
              << std::endl;
}

/* エラーメッセージを表示 */
void mutex_logger::caution(const std::string& msg, const std::string& suppl){ 
    std::lock_guard<std::mutex> log_lock(mutex_logger::lock);
    std::cerr << RED
              << "[Error] "
              << RESET
              << msg
              << ". ("
              << suppl
              << ")"
              << std::endl;
}

/* String型変数を表示 */
void mutex_logger::debug(const std::string& msg){ 
    std::lock_guard<std::mutex> log_lock(mutex_logger::lock);
    std::cout << CYAN
              << "[Debug] "
              << RESET
              << msg
              << std::endl;
}

/* int型変数を表示 */
void mutex_logger::debug(const int num){
    std::lock_guard<std::mutex> log_lock(mutex_logger::lock);
    std::cout << CYAN
              << "[Debug] "
              << RESET
              << std::to_string(num)
              << std::endl;
}

/* double型変数を表示 */
void mutex_logger::debug(const double num){
    std::lock_guard<std::mutex> log_lock(mutex_logger::lock);
    std::cout << CYAN
              << "[Debug] "
              << RESET
              << std::to_string(num)
              << std::endl;
}

