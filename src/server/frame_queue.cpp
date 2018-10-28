/*******************************
 *       frame_queue.cpp       *
 *    (分割フレーム用キュー)   *
 *******************************/

#include "frame_queue.hpp"

/* コンストラクタ */
FrameQueue::FrameQueue(const int max_size):
    max_size(max_size)
{}

/* デストラクタ */
FrameQueue::~FrameQueue(){}

/* キューにフレームを投入 */
void FrameQueue::enqueue(const cv::Mat &frame){
    boost::unique_lock<boost::mutex> u_lock(this->lock);
    const bool was_empty = this->queue.empty();
    this->queue.push(frame);
    if(was_empty){
        this->cond.notify_one();
    }
    return;
}

/* キューからフレームを取り出し */
cv::Mat FrameQueue::dequeue(){
    boost::unique_lock<boost::mutex> u_lock(this->lock);
    while(this->queue.empty()){
        this->cond.wait(u_lock);
    }
    const cv::Mat frame = this->queue.front();
    this->queue.pop();
    return frame;
}

/* キュー内のフレーム数を取得 */
const std::size_t FrameQueue::getSize(){
    boost::unique_lock<boost::mutex> u_lock(this->lock);
    return this->queue.size();
}

