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
        int max_size;                            // キューの容量
        std::queue<cv::Mat> queue;               // キュー
        mutable boost::mutex lock;               // 排他制御用のロック
        mutable boost::condition_variable cond;  // 条件変数
    public:
        FrameQueue(int max_size);            // コンストラクタ
        ~FrameQueue();                       // デストラクタ
        void enqueue(const cv::Mat &frame);  // キューにフレームを投入
        cv::Mat dequeue();                   // キューからフレームを取り出し
        const std::size_t getSize();         // キュー内のフレーム数を取得
};

/* 変数型の定義 */
using smt_FrameQueue_t = std::shared_ptr<FrameQueue>;

#endif  /* FRAME_QUEUE_HPP */

