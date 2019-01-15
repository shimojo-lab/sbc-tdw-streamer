/*************************
*    frame_sender.cpp    *
*    (フレーム送信器)    *
*************************/

#include "frame_sender.hpp"

/* コンストラクタ */
FrameSender::FrameSender(_asio::io_service& ios, const int port, const int display_num,
                         std::vector<tranbuf_ptr_t>& send_bufs, const int dec_thre_num):
    ios(ios),
    acc(ios, _ip::tcp::endpoint(_ip::tcp::v4(), port)),
    display_num(display_num),
    viewbuf_num(dec_thre_num+1),
    send_msgs(display_num),
    send_bufs(send_bufs)
{
    // パラメータを初期化
    this->sock = std::make_shared<_ip::tcp::socket>(ios);
    this->send_count.store(0);
    
    // 送信処理を開始
    _ml::notice("Streaming video frames at :" + std::to_string(port));
    this->run();
}

/* 送信処理を開始 */
void FrameSender::run(){
    this->acc.async_accept(*this->sock,
                           boost::bind(&FrameSender::onConnect, this, _ph::error)
    );
    this->ios.run();
}

/* フレームを送信 */
void FrameSender::sendFrame(){
    for(int i=0; i<this->display_num; ++i){
        this->send_msgs[i] = this->send_bufs[i]->pop() + std::to_string(this->fb_id) + MSG_DELIMITER;
        _asio::async_write(*this->socks[i],
                           _asio::buffer(this->send_msgs[i]),
                           boost::bind(&FrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred)
        );
    }
    this->fb_id = (this->fb_id+1) % this->viewbuf_num;
}

/* ディスプレイノード接続時のコールバック */
void FrameSender::onConnect(const err_t& err){
    const std::string ip = this->sock->remote_endpoint().address().to_string();
    if(err){
        _ml::caution("Failed stream connection with " + ip, err.message());
        std::exit(EXIT_FAILURE);
    }else{
        ++this->send_count;
    }
    
    // 新規TCPソケットを用意
    this->socks.push_back(this->sock);
    this->sock = std::make_shared<_ip::tcp::socket>(this->ios);
    
    if(this->send_count.load(std::memory_order_acquire) < this->display_num){
        // 接続待機を再開
        this->acc.async_accept(*this->sock,
                               boost::bind(&FrameSender::onConnect, this, _ph::error)
        );
    }else{
        // フレーム送信を開始
        this->sock->close();
        this->send_count.store(0);
        this->sendFrame();
    }
}

/* フレーム送信時のコールバック */
void FrameSender::onSendFrame(const err_t& err, size_t t_bytes){
    if(err){
        _ml::caution("Failed to send frame", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 全送信が完了したら次番フレームを送信
    ++this->send_count;
    if(this->send_count.load(std::memory_order_acquire) == this->display_num){
        this->send_count.store(0);
        this->sendFrame();
    }
}

