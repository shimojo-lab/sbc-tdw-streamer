/*********************************
*      print_with_mutex.cpp      *
*  (排他制御付きコンソール出力)  *
*********************************/

#include "print_with_mutex.hpp"

/* コンソール出力用の排他制御を実行 */
std::unique_lock<std::mutex> console_lock(){
    static std::mutex lock;
    return std::unique_lock<std::mutex>(lock);
}

/* 一般メッセージを表示 */
void print_info(const std::string& msg){
    console_lock(), std::cout << "[Info] "
                              << msg
                              << "."
                              << std::endl;
}

/* 一般メッセージを表示 */
void print_warn(const std::string& msg){
    console_lock(), std::cout << "[Warning] "
                              << msg
                              << "."
                              << std::endl;
}

/* エラーメッセージを表示 */
void print_err(const std::string& msg, const std::string& suppl){
    console_lock(), std::cerr << "[Error] "
                              << msg
                              << ". ("
                              << suppl
                              << ")"
                              << std::endl;
}

/* デバッグ用メッセージを表示 (文字列用) */
void print_debug(const std::string& msg){
    console_lock(), std::cout << "[Debug] "
                              << msg
                              << std::endl;
}

/* デバッグ用メッセージを表示 (int型変数用) */
void print_debug(const int& num){
    console_lock(), std::cout << "[Debug] "
                              << std::to_string(num)
                              << std::endl;
}

/* デバッグ用メッセージを表示 (double型変数用) */
void print_debug(const double& num){
    console_lock(), std::cout << "[Debug] "
                              << std::to_string(num)
                              << std::endl;
}
