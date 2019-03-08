/***********************************
*         mutex_logger.cpp         *
*  (the logging tools with mutex)  *
***********************************/

#include "mutex_logger.hpp"

/* print a notice on the console */
void mutex_logger::notice(const std::string& msg){
    std::lock_guard<std::mutex> log_lock(_ml::lock);
    std::cout << GREEN
              << "[Info] "
              << RESET
              << msg
              << "."
              << std::endl;
}

/* print a warning on the console */
void mutex_logger::warn(const std::string& msg, const std::string& suppl){
    std::lock_guard<std::mutex> log_lock(_ml::lock);
    std::cout << YELLOW
              << "[Warning] "
              << RESET
              << msg
              << ". ("
              << suppl
              << ")"
              << std::endl;
}

/* print a caution on the console */
void mutex_logger::caution(const std::string& msg, const std::string& suppl){ 
    std::lock_guard<std::mutex> log_lock(_ml::lock);
    std::cerr << RED
              << "[Error] "
              << RESET
              << msg
              << ". ("
              << suppl
              << ")"
              << std::endl;
}

/* print a string variable for debug */
void mutex_logger::debug(const std::string& var){ 
    std::lock_guard<std::mutex> log_lock(_ml::lock);
    std::cout << CYAN
              << "[Debug] "
              << RESET
              << var
              << std::endl;
}

/* print a int variable for debug */
void mutex_logger::debug(const int var){
    std::lock_guard<std::mutex> log_lock(_ml::lock);
    std::cout << CYAN
              << "[Debug] "
              << RESET
              << std::to_string(var)
              << std::endl;
}

/* print a double variable for debug */
void mutex_logger::debug(const double var){
    std::lock_guard<std::mutex> log_lock(_ml::lock);
    std::cout << CYAN
              << "[Debug] "
              << RESET
              << std::to_string(var)
              << std::endl;
}

