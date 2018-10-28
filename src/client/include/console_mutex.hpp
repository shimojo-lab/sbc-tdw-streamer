/******************************
 *     console_mutex.hpp      *
 * (コンソール出力の排他制御) *
 ******************************/

#ifndef CONSOLE_MUTEX_HPP
#define CONSOLE_MUTEX_HPP

#include <boost/thread.hpp>

/* コンソール出力用の排他制御を実行 */
inline boost::unique_lock<boost::mutex> c_lock(){
    static boost::mutex lock;
    return boost::unique_lock<boost::mutex>(lock);
}

#endif  /* CONSOLE_MUTEX_HPP */

