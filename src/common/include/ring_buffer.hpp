/*********************************
*        ring_buffer.hpp         *
*  (排他制御付きリングバッファ)  *
*********************************/

#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <mutex>
#include <condition_variable>
#include <boost/circular_buffer.hpp>

const int STATIC_BUF = 0;         // 固定サイズのバッファタイプ
const int DYNAMIC_BUF = 1;        // 可変サイズのバッファタイプ
const int BUF_FRONT_INDEX = 0;    // バッファの先頭のインデックス
const int COLOR_CHANNEL_NUM = 3;  // フレームの色チャネル数
const int VIEWBUF_ID_LEN = 1;     // 表示フレームバッファのインデックス長

/* 排他制御付きリングバッファ */
template <typename T>
class RingBuffer{
    private:
        const int type;                        // バッファタイプ
        const int init_size;                   // 初期バッファサイズ
        boost::circular_buffer<T> buf;         // リングバッファ
        mutable std::mutex lock;               // 排他制御用のロック
        mutable std::condition_variable cond;  // 条件変数
     
    public:
        RingBuffer(const int type, const int init_size);  // コンストラクタ
        void push(const T& item);                         // 末尾にアイテムを投入
        const T pop();                                    // 先頭からアイテムを取り出し
        const int getItemNum();                           // バッファ内のアイテム数を取得
};

/* コンストラクタ */
template <typename T>
RingBuffer<T>::RingBuffer(const int type, const int init_size):
    type(type),
    init_size(init_size),
    buf(init_size)
{}

/* 末尾にアイテムを投入 */
template <typename T>
void RingBuffer<T>::push(const T& item){
    // 可変サイズなら必要に応じてサイズ拡張
    std::unique_lock<std::mutex> u_lock(this->lock);
    if(this->buf.full() && this->type==DYNAMIC_BUF){
        this->buf.set_capacity(this->buf.size()+this->init_size);
    }
    
    // アイテムを投入し、バッファ枯渇時のロックを解除
    const bool was_empty = this->buf.empty();
    this->buf.push_back(item);
    if(was_empty){
        this->cond.notify_one();
    }
}

/* 先頭からアイテムを取り出し */
template <typename T>
const T RingBuffer<T>::pop(){
    // バッファが枯渇していたら待機
    std::unique_lock<std::mutex> u_lock(this->lock);
    while(this->buf.empty()){
        this->cond.wait(u_lock);
    }
    
    // アイテムを取り出し
    const T item = this->buf[BUF_FRONT_INDEX];
    this->buf.pop_front();
    return item;
}

/* バッファ内のフレーム数を取得 */
template <typename T>
const int RingBuffer<T>::getItemNum(){
    std::unique_lock<std::mutex> u_lock(this->lock);
    return this->buf.size();
}

using jpegbuf_ptr_t = std::shared_ptr<RingBuffer<std::string>>;
using ucharbuf_ptr_t = std::shared_ptr<RingBuffer<unsigned char*>>;

#endif  /* RING_BUFFER_HPP */

