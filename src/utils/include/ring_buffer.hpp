/**************************
 *    ring_buffer.hpp     *
 *    (リングバッファ)    *
 *************************/

#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <mutex>
#include <condition_variable>
#include <memory>
#include <boost/circular_buffer.hpp>

/* リングバッファ */
template <typename T>
class RingBuffer{
    private:
        boost::circular_buffer<T> buf;         // リングバッファ
        mutable std::mutex lock;               // 排他制御用のロック
        mutable std::condition_variable cond;  // 条件変数
    
    public:
        RingBuffer(const int buf_size);  // コンストラクタ
        void push(const T& item);        // キューにフレームを投入
        const T pop();                   // キューからフレームを取り出し
        const int getSize();             // キュー内のフレーム数を取得
};

/* コンストラクタ */
template <typename T>
RingBuffer<T>::RingBuffer(const int buf_size):
    buf(buf_size)
{}

/* 末尾にアイテムを投入 */
template <typename T>
void RingBuffer<T>::push(const T& item){
    std::unique_lock<std::mutex> u_lock(this->lock);
    if(this->buf.full()){
        this->buf.set_capacity(this->buf.size()*2);
    }
    const bool was_empty = this->buf.empty();
    this->buf.push_back(item);
    if(was_empty){
        this->cond.notify_one();
    }
}

/* 先頭から取り出し */
template <typename T>
const T RingBuffer<T>::pop(){
    std::unique_lock<std::mutex> u_lock(this->lock);
    while(this->buf.empty()){
        this->cond.wait(u_lock);
    }
    const T item = this->buf[0];
    this->buf.pop_front();
    return item;
}

/* バッファ内フレーム数を取得 */
template <typename T>
const int RingBuffer<T>::getSize(){
    std::unique_lock<std::mutex> u_lock(this->lock);
    return this->buf.size();
}

using msgbuf_ptr_t = std::shared_ptr<RingBuffer<std::string>>;
using framebuf_ptr_t = std::shared_ptr<RingBuffer<std::string>>;

#endif  /* RING_BUFFER_HPP */

