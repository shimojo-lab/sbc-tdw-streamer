/******************************
*   viewer_synchronizer.cpp   *
*   (表示タイミング制御器)    *
******************************/

#include "viewer_synchronizer.hpp"

/* コンストラクタ */
ViewerSynchronizer::ViewerSynchronizer(ios_t& ios, std::vector<tcps_ptr_t>& sock_list):
    ios(ios),
    strand(ios),
    sock_list(sock_list),
    display_num(sock_list.size())
{
    // 受信用バッファを初期化
    this->buf_list = std::vector<buf_ptr_t>(this->display_num);
    for(int id=0; id<this->display_num; ++id){
        this->buf_list[id].reset(new _asio::streambuf());
    }
    
    // 同期済ディスプレイ数を初期化
    this->sync_count = 0;
}

/* 同期メッセージ受信時のコールバック */
void ViewerSynchronizer::onRecvSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        print_err("Failed to receive sync message", err.message());
    }
    
    // 同期メッセージをパース
    this->buf_list[id]->consume(t_bytes);
    
    // 全ディスプレイノード間で同期
/*    ++this->sync_count;
    if(this->sync_count.load(std::memory_order_acquire) == this->display_num){
        this->sync_count = 0;*/
        this->sendSync();
//    }
}

/* 同期メッセージ送信時のコールバック */
void ViewerSynchronizer::onSendSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        print_err("Failed to send sync message", err.message());
    }
    
    // 同期メッセージ受信を再開
    const auto bind = boost::bind(&ViewerSynchronizer::onRecvSync, this, _ph::error, _ph::bytes_transferred, id);
    _asio::async_read_until(*this->sock_list[id], *this->buf_list[id], SEPARATOR, bind);
}

/* 同期メッセージを送信 */
void ViewerSynchronizer::sendSync(){
    std::string send_msg("sync");
    send_msg += SEPARATOR;
    for(int id=0; id<this->display_num; ++id){
        const auto bind = boost::bind(&ViewerSynchronizer::onSendSync, this, _ph::error, _ph::bytes_transferred, id);
        _asio::async_write(*this->sock_list[id], _asio::buffer(send_msg), bind);
    }
}

/* 同期メッセージの受信を開始 */
void ViewerSynchronizer::run(){
    for(int id=0; id<this->display_num; ++id){
        const auto bind = boost::bind(&ViewerSynchronizer::onRecvSync, this, _ph::error, _ph::bytes_transferred, id);
        _asio::async_read_until(*this->sock_list[id], *this->buf_list[id], SEPARATOR, bind);
    }
}

