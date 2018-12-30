/*************************
*    sync_manager.cpp    *
*      (同期制御器)      *
*************************/

#include "sync_manager.hpp"

/* コンストラクタ */
SyncManager::SyncManager(_asio::io_service& ios, std::vector<sock_ptr_t>& socks,
                         std::atomic<int>& quality, std::atomic<bool>& send_semaphore):
    ios(ios),
    socks(socks),
    display_num(socks.size()),
    quality(quality),
    send_semaphore(send_semaphore)
{
    // パラメータを初期化
    this->sync_count.store(0, std::memory_order_release);
    for(int id=0; id<this->display_num; ++id){
        this->stream_bufs.push_back(std::make_shared<_asio::streambuf>());
    }
}

/* 同期メッセージをパース */
void SyncManager::parseSync(std::string& recv_msg){
    // メモリ残量の情報を取得
    for(int i=0; i<MSG_DELIMITER_LEN; ++i){
        recv_msg.pop_back();
    }
    const int mem_state = recv_msg.back() - '0';
    if(mem_state != int(this->send_semaphore.load(std::memory_order_acquire))){
        this->send_semaphore.store(bool(mem_state), std::memory_order_release);
    }
    recv_msg.pop_back();
}

/* 同期メッセージ受信時のコールバック */
void SyncManager::onRecvSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        print_err("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージをパース
    const auto data = this->stream_bufs[id]->data();
    std::string recv_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    this->parseSync(recv_msg);
    this->stream_bufs[id]->consume(t_bytes);
    
    // 全ディスプレイノード間で同期
    const int count = this->sync_count.load(std::memory_order_acquire);
    this->sync_count.store(count+1, std::memory_order_release);
    if(this->sync_count.load(std::memory_order_acquire) == this->display_num){
        this->sync_count.store(0, std::memory_order_release);
        this->sendSync();
    }
}

/* 同期メッセージ送信時のコールバック */
void SyncManager::onSendSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        print_err("Failed to send sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージ受信を再開
    _asio::async_read_until(*this->socks[id],
                            *this->stream_bufs[id],
                            MSG_DELIMITER,
                            boost::bind(&SyncManager::onRecvSync, this, _ph::error, _ph::bytes_transferred, id)
    );
}

/* 同期メッセージを送信 */
void SyncManager::sendSync(){
    std::string send_msg("sync");
    send_msg += MSG_DELIMITER;
    for(int id=0; id<this->display_num; ++id){
        _asio::async_write(*this->socks[id],
                           _asio::buffer(send_msg),
                           boost::bind(&SyncManager::onSendSync, this, _ph::error, _ph::bytes_transferred, id)
        );
    }
}

/* 同期メッセージの受信を開始 */
void SyncManager::run(){
    for(int id=0; id<this->display_num; ++id){
        _asio::async_read_until(*this->socks[id],
                                *this->stream_bufs[id],
                                MSG_DELIMITER,
                                boost::bind(&SyncManager::onRecvSync, this, _ph::error, _ph::bytes_transferred, id)
        );
    }
    this->ios.run();
}

