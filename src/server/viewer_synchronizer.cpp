/******************************
*   viewer_synchronizer.cpp   *
*        (同期制御器)         *
******************************/

#include "viewer_synchronizer.hpp"

/* コンストラクタ */
ViewerSynchronizer::ViewerSynchronizer(ios_t& ios, std::vector<tcps_ptr_t>& sock_list, std::atomic<int>& comp_quality):
    ios(ios),
    sock_list(sock_list),
    display_num(sock_list.size()),
    comp_quality(comp_quality)
{
    this->sync_count.store(0, std::memory_order_release);
    for(int id=0; id<this->display_num; ++id){
        this->streambuf_list.push_back(std::make_shared<_asio::streambuf>());
    }
}

/* 同期メッセージ受信時のコールバック */
void ViewerSynchronizer::onRecvSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        print_err("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージをパース
    this->streambuf_list[id]->consume(t_bytes);
    
    // 全ディスプレイノード間で同期
    const int count = this->sync_count.load(std::memory_order_acquire);
    this->sync_count.store(count+1, std::memory_order_release);
    if(this->sync_count.load(std::memory_order_acquire) == this->display_num){
        this->sync_count.store(0, std::memory_order_release);
        this->sendSync();
    }
}

/* 同期メッセージ送信時のコールバック */
void ViewerSynchronizer::onSendSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        print_err("Failed to send sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージ受信を再開
    const auto bind = boost::bind(&ViewerSynchronizer::onRecvSync, this, _ph::error, _ph::bytes_transferred, id);
    _asio::async_read_until(*this->sock_list[id], *this->streambuf_list[id], MSG_DELIMITER, bind);
}

/* 同期メッセージを送信 */
void ViewerSynchronizer::sendSync(){
    std::string send_msg("sync");
    send_msg += MSG_DELIMITER;
    for(int id=0; id<this->display_num; ++id){
        const auto bind = boost::bind(&ViewerSynchronizer::onSendSync, this, _ph::error, _ph::bytes_transferred, id);
        _asio::async_write(*this->sock_list[id], _asio::buffer(send_msg), bind);
    }
}

/* 同期メッセージの受信を開始 */
void ViewerSynchronizer::run(){
    for(int id=0; id<this->display_num; ++id){
        const auto bind = boost::bind(&ViewerSynchronizer::onRecvSync, this, _ph::error, _ph::bytes_transferred, id);
        _asio::async_read_until(*this->sock_list[id], *this->streambuf_list[id], MSG_DELIMITER, bind);
    }
    this->ios.run();
}

