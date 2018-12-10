/*****************************
*     memory_checker.cpp     *
*     (メモリ残量確認器)     *
*****************************/

#include "memory_checker.hpp"

/* コンストラクタ */
MemoryChecker::MemoryChecker(const double threshold):
    threshold(threshold)
{
    sysinfo(&this->info);
    this->total_ram = (double)this->info.totalram;
}

/* メモリ残量不足を判定 */
const bool MemoryChecker::checkShortage(){
    sysinfo(&this->info);
    const double used_rate = 1.0 - (double)this->info.freeram / this->total_ram;
    return (used_rate <= this->threshold);
}

