/*****************************
*     memory_checker.hpp     *
*     (メモリ残量確認器)     *
*****************************/

#ifndef MEMORY_CHECKER_HPP
#define MEMORY_CHECKER_HPP

#include "print_with_mutex.hpp"
extern "C"{
    #include <sys/sysinfo.h>
}

/* メモリ残量確認器 */
class MemoryChecker{
    private:
        struct sysinfo info = {0};  // メモリ情報
        double total_ram;           // 全メモリ容量
        const double threshold;     // 最大メモリ利用率
        
    public:
        MemoryChecker(const double threshold);  // コンストラクタ
        bool checkShortage();                   // メモリ残量不足を判定
};

#endif  /* MEMORY_CHECKER_HPP */

