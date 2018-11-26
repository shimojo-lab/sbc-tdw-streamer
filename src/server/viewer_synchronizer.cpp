/******************************
*   viewer_synchronizer.cpp   *
*   (表示タイミング制御器)    *
******************************/

#include "viewer_synchronizer.hpp"
#include <stdio.h>
/* コンストラクタ */
ViewerSynchronizer::ViewerSynchronizer(ios_t& ios, std::vector<tcps_ptr_t>& sock_list, tcp_t::acceptor& acc):
    ios(ios),
    sock_list(sock_list),
    acc(acc)
{
    // 同期用バリアを初期化
    this->display_num = sock_list.size();
    this->barrier = std::make_shared<boost::barrier>(this->display_num);
}

/* 同期メッセージ受信時のコールバック */
void ViewerSynchronizer::onRecvSync(const err_t& err, size_t t_bytes, const int i){
    if(err){
        print_err("Failed to receive sync message", err.message());
    }
    
    // 全ディスプレイノード間で同期
    this->barrier->wait();
    const auto bind = boost::bind(&ViewerSynchronizer::onSendSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(*this->sock_list[i], _asio::buffer(SEPARATOR), bind);
}

/* 同期メッセージ送信時のコールバック */
void ViewerSynchronizer::onSendSync(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to send sync message", err.message());
    }
    this->run();
}

/* 同期メッセージの受信を開始 */
void ViewerSynchronizer::run(){
    for(int i=0; i<this->display_num; ++i){
        const auto bind = boost::bind(&ViewerSynchronizer::onRecvSync, this, _ph::error, _ph::bytes_transferred, i);
        _asio::async_read_until(*this->sock_list[i], this->recv_buf, SEPARATOR, bind);
    }
}

