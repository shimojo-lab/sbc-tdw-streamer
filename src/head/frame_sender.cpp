/*************************
*    frame_sender.cpp    *
*    (フレーム送信器)    *
*************************/

#include "frame_sender.hpp"

/* コンストラクタ */
FrameSender::FrameSender(_asio::io_service& ios, const int port, const int display_num,
                         std::atomic<bool>& send_semaphore, std::vector<jpegbuf_ptr_t>& send_bufs):
    ios(ios),
    acc(ios, _ip::tcp::endpoint(_ip::tcp::v4(), port)),
    display_num(display_num),
    send_semaphore(send_semaphore),
    send_bufs(send_bufs)
{
    // パラメータを初期化
    this->sock = std::make_shared<_ip::tcp::socket>(ios);
    this->send_count.store(0, std::memory_order_release);
    
    // 送信処理を開始
    print_info("Streaming video frames at :" + std::to_string(port));
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
    for(int id=0; id<this->display_num; ++id){    
//        const std::string param = PARAM_DELIMITER + std::to_string(this->frame_num);
        const std::string param = "";
        const std::string send_msg = this->send_bufs[id]->pop() + param + MSG_DELIMITER;
        _asio::async_write(*this->socks[id],
                           _asio::buffer(send_msg),
                           boost::bind(&FrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred));
    }
    this->frame_num += 1;
}

/* ディスプレイノード接続時のコールバック */
void FrameSender::onConnect(const err_t& err){
    const std::string ip = this->sock->remote_endpoint().address().to_string();
    if(err){
        print_err("Failed stream connection with " + ip, err.message());
        std::exit(EXIT_FAILURE);
    }else{
        const int count = this->send_count.load(std::memory_order_acquire);
        this->send_count.store(count+1, std::memory_order_release);
    }
    
    // 新規TCPソケットを用意
    this->socks.push_back(this->sock);
    this->sock = std::make_shared<_ip::tcp::socket>(this->ios);
    
    if(this->send_count.load(std::memory_order_acquire) < this->display_num){
        // 接続待機を再開
        const auto bind = boost::bind(&FrameSender::onConnect, this, _ph::error);
        this->acc.async_accept(*this->sock, bind);
    }else{
        // フレーム送信を開始
        this->sock->close();
        this->send_count.store(0, std::memory_order_release);
        this->sendFrame();
    }
}

/* フレーム送信時のコールバック */
void FrameSender::onSendFrame(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to send frame", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 全送信が完了したら次番フレームを送信
    const int count = this->send_count.load(std::memory_order_acquire);
    this->send_count.store(count+1, std::memory_order_release);
    if(this->send_count.load(std::memory_order_acquire) == this->display_num){
        this->send_count.store(0, std::memory_order_release);
        
        // 受信側のメモリ残量に応じて送信中断
        while(!this->send_semaphore.load(std::memory_order_acquire)){
            std::this_thread::sleep_for(std::chrono::seconds(SEND_WAIT_TIME));
        }
        this->sendFrame();
    }
}

