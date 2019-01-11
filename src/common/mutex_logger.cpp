/*****************************
*      mutex_logger.cpp      *
*    (排他制御付きロガー)    *
*****************************/

#include "mutex_logger.hpp"

/* 排他制御を実行 */
std::unique_lock<std::mutex> mutex_logger::log_lock(){
    static std::mutex lock;
    return std::unique_lock<std::mutex>(lock);
}

/* 通知メッセージを表示 */
void mutex_logger::notice(const std::string& msg){
    mutex_logger::log_lock(), std::cout << GREEN
                                        << "[Info] "
                                        << RESET
                                        << msg
                                        << "."
                                        << std::endl;
}

/* 警告メッセージを表示 */
void mutex_logger::warn(const std::string& msg, const std::string& suppl){
    mutex_logger::log_lock(), std::cout << YELLOW
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
    mutex_logger::log_lock(), std::cerr << RED
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
    mutex_logger::log_lock(), std::cout << CYAN
                                        << "[Debug] "
                                        << RESET
                                        << msg
                                        << std::endl;
}

/* int型変数を表示 */
void mutex_logger::debug(const int num){
    mutex_logger::log_lock(), std::cout << CYAN
                                        << "[Debug] "
                                        << RESET
                                        << std::to_string(num)
                                        << std::endl;
}

/* double型変数を表示 */
void mutex_logger::debug(const double num){
    mutex_logger::log_lock(), std::cout << CYAN
                                        << "[Debug] "
                                        << RESET
                                        << std::to_string(num)
                                        << std::endl;
}

