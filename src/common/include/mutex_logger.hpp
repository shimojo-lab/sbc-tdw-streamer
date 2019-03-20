/*******************************
*       mutex_logger.hpp       *
*  (logging tools with mutex)  *
*******************************/

#ifndef MUTEX_LOGGER_HPP
#define MUTEX_LOGGER_HPP

#include <iostream>
#include <string>
#include <mutex>

const std::string GREEN("\033[0;32m");   // the declaration to use green
const std::string YELLOW("\033[0;33m");  // the declaration to use yellow
const std::string RED("\033[0;31m");     // the declaration to use red
const std::string CYAN("\033[0;36m");    // the declaration to use cyan
const std::string RESET("\033[0m");      // the declaration to reset color

/* logging tools with mutex */
namespace mutex_logger{
    static std::mutex lock;                                          // the mutex lock
    void notice(const std::string& msg);                             // print a notice
    void warn(const std::string& msg, const std::string& suppl);     // print a warning
    void caution(const std::string& msg, const std::string& suppl);  // print a caution
    void debug(const std::string& var);                              // print a string variable
    void debug(const int var);                                       // print a int variable
    void debug(const double var);                                    // print a double variable
}

namespace _ml = mutex_logger;

#endif  /* MUTEX_LOGGER_HPP */

