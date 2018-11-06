/*******************************
 *       frame_queue.hpp       *
 *    (分割フレーム用キュー)   *
 *******************************/

#ifndef FRAME_QUEUE_HPP
#define FRAME_QUEUE_HPP

#include <queue>
#include <memory>
#include <boost/thread.hpp>
#include <opencv2/core.hpp>

/* 分割フレーム用キュー */
class FrameQueue{
    private:
        const std::size_t max_size;              // キューの容量
        std::queue<cv::Mat> queue;               // キュー
        mutable boost::mutex lock;               // 排他制御用のロック
        mutable boost::condition_variable cond;  // 条件変数
    
    public:
        FrameQueue(const std::size_t max_size);  // コンストラクタ
        void enqueue(const cv::Mat& frame);      // キューにフレームを投入
        const cv::Mat dequeue();                 // キューからフレームを取り出し
        const std::size_t getSize();             // キュー内のフレーム数を取得
};

using fq_ptr_t = std::shared_ptr<FrameQueue>;  // FrameQueue用のsharedポインタ

#endif  /* FRAME_QUEUE_HPP */

