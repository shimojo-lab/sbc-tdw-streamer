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
        const int max_size;                            // キューの容量
        std::queue<std::vector<unsigned char>> queue;  // キュー
        mutable boost::mutex lock;                     // 排他制御用のロック
        mutable boost::condition_variable cond;        // 条件変数
    public:
        FrameQueue(const int max_size);                         // コンストラクタ
        ~FrameQueue();                                          // デストラクタ
        void enqueue(const std::vector<unsigned char> &frame);  // キューにフレームを投入
        std::vector<unsigned char> dequeue();                   // キューからフレームを取り出し
        const std::size_t getSize();                            // キュー内のフレーム数を取得
};

using smt_fq_t = std::shared_ptr<FrameQueue>;  // FrameQueueのsharedポインタ

#endif  /* FRAME_QUEUE_HPP */

