/*********************************
*      print_with_mutex.cpp      *
*  (排他制御付きコンソール出力)  *
*********************************/

#include "print_with_mutex.hpp"

/* コンソール出力用の排他制御を実行 */
boost::unique_lock<boost::mutex> console_lock(){
    static boost::mutex lock;
    return boost::unique_lock<boost::mutex>(lock);
}

/* 一般メッセージを表示 */
void print_info(const std::string &msg){
    console_lock(), std::cout << "[Info] "
                              << msg
                              << "."
                              << std::endl;
    return;
}

/* エラーメッセージを表示 */
void print_err(const std::string &msg, const std::string &err_msg){
    console_lock(), std::cerr << "[Error] "
                              << msg
                              << ". ("
                              << err_msg
                              << ")"
                              << std::endl;
    return;
}

