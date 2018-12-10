/****************************
*   tcp_frame_sender.cpp    *
*   (TCP版フレーム送信器)   *
****************************/

#include "tcp_frame_sender.hpp"

/* コンストラクタ */
TCPFrameSender::TCPFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf, const int port, const int display_num, std::atomic<bool>& send_semaphore):
    BaseFrameSender(ios, sbuf, display_num, send_semaphore),
    acc(ios, tcp_t::endpoint(tcp_t::v4(), port))
{
    // パラメータを初期化
    this->sock = std::make_shared<tcp_t::socket>(ios);
    this->send_count.store(0, std::memory_order_release);
    
    // 送信処理を開始
    print_info("Started TCP frame streaming at :" + std::to_string(port));
    this->run();
}

/* 送信処理を開始 */
void TCPFrameSender::run(){
    // ディスプレイノードからの再接続を受理
    const auto bind = boost::bind(&TCPFrameSender::onConnect, this, _ph::error);
    this->acc.async_accept(*this->sock, bind);
    this->ios.run();
}

/* ディスプレイノード接続時のコールバック */
void TCPFrameSender::onConnect(const err_t& err){
    // 接続元を表示
    const std::string ip = this->sock->remote_endpoint().address().to_string();
    if(err){
        print_err("Failed TCP connection with " + ip, err.message());
        std::exit(EXIT_FAILURE);
    }else{
        const int count = this->send_count.load(std::memory_order_acquire);
        this->send_count.store(count+1, std::memory_order_release);
    }
    print_info("Established TCP connection with " + ip);
    
    // 新規TCPソケットを用意
    this->sock_list.push_back(this->sock);
    this->sock = std::make_shared<tcp_t::socket>(this->ios);
    
    if(this->send_count.load(std::memory_order_acquire) < this->display_num){
        // 接続待機を再開
        const auto bind = boost::bind(&TCPFrameSender::onConnect, this, _ph::error);
        this->acc.async_accept(*this->sock, bind);
    }else{
        // フレーム送信を開始
        this->sock->close();
        this->send_count.store(0, std::memory_order_release);
        this->sendFrame();
    }
}

/* フレームを送信 */
void TCPFrameSender::sendFrame(){
    // 送信メッセージを作成
    const std::string seq = SEC_DELIMITER + std::to_string(this->sequence_num);
    const std::string send_msg = this->sbuf->pop() + seq + MSG_DELIMITER;
    ++this->sequence_num;
    
    // 全ディスプレイノードへ送信
    const auto bind = boost::bind(&TCPFrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    for(int id=0; id<this->display_num; ++id){
        _asio::async_write(*this->sock_list[id], _asio::buffer(send_msg), bind);
    }
}

/* フレーム送信時のコールバック */
void TCPFrameSender::onSendFrame(const err_t& err, size_t t_bytes){
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

