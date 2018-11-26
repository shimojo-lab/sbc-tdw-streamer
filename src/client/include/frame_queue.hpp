/********************************
 *       frame_queue.hpp        *
 *    (分割フレーム用キュー)    *
 ********************************/

#ifndef FRAME_QUEUE_HPP
#define FRAME_QUEUE_HPP

#include <queue>
#include <vector>
#include <memory>
#include <boost/thread.hpp>

/* 分割フレーム用キュー */
class FrameQueue{
    private:
        const size_t max_size;                         // キューの容量
        std::queue<std::vector<unsigned char>> queue;  // キュー
        mutable boost::mutex lock;                     // 排他制御用のロック
        mutable boost::condition_variable cond;        // 条件変数
    
    public:
        FrameQueue(const size_t max_size);                      // コンストラクタ
        void enqueue(const std::vector<unsigned char>& frame);  // キューにフレームを投入
        const std::vector<unsigned char> dequeue();             // キューからフレームを取り出し
        const size_t getSize();                                 // キュー内のフレーム数を取得
};

using fq_ptr_t = std::shared_ptr<FrameQueue>;  // FrameQueueのsharedポインタ

#endif  /* FRAME_QUEUE_HPP */

