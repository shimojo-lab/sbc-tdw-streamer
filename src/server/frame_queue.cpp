/*******************************
 *       frame_queue.cpp       *
 *    (分割フレーム用キュー)   *
 *******************************/

#include "frame_queue.hpp"

/* コンストラクタ */
FrameQueue::FrameQueue(const std::size_t max_size):
    max_size(max_size)
{}

/* キューにフレームを投入 */
void FrameQueue::enqueue(const cv::Mat& frame){
    boost::unique_lock<boost::mutex> u_lock(this->lock);
/*    while(this->queue.size() >= this->max_size){
        this->cond.wait(u_lock);
    }
    const bool was_empty = this->queue.empty();
    this->queue.push(frame);
    if(was_empty){
        this->cond.notify_one();
    }*/
    if(this->queue.empty()){
        this->queue.push(frame);
        this->cond.notify_one();
    }else{
        while(this->queue.size() >= this->max_size){
            this->cond.wait(u_lock);
        }
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
    if(this->queue.size() < this->max_size){
        this->cond.notify_one();
    }
    return frame;
}

/* キュー内のフレーム数を取得 */
const std::size_t FrameQueue::getSize(){
    boost::unique_lock<boost::mutex> u_lock(this->lock);
    return this->queue.size();
}

