/****************************
*   udp_frame_sender.cpp    *
*   (UDP版フレーム送信器)   *
****************************/

#include "udp_frame_sender.hpp"

/* コンストラクタ */
UDPFrameSender::UDPFrameSender(ios_t& ios, const msgbuf_ptr_t sbuf, const int port, const int display_num, std::vector<std::string>& ip_list):
    BaseFrameSender(ios, sbuf),
    sock(ios, udp_t::v4()),
    display_num(display_num)
{
    for(const std::string& ip : ip_list){
        this->endpoint_list.push_back(udp_t::endpoint(_ip::address::from_string(ip), port));
    }
//    this->sock.set_option(_asio::socket_base::send_buffer_size(8192));
    this->send_count.store(0, std::memory_order_release);
    print_info("Started UDP frame streaming");
    this->run();
}

/* 送信処理を開始 */
void UDPFrameSender::run(){
    this->sendFrame();
    this->ios.run();
}

/* フレームを送信 */
void UDPFrameSender::sendFrame(){
    // 送信メッセージを作成
    const std::string str_seq = SEQ_DELIMITER + std::to_string(this->sequence_num);
    const std::string send_msg = this->sbuf->pop() + str_seq + MSG_DELIMITER;
    ++this->sequence_num;
    
    // 全ディスプレイノードへ送信
    const auto bind = boost::bind(&UDPFrameSender::onSendFrame, this, _ph::error, _ph::bytes_transferred);
    for(int id=0; id<this->display_num; ++id){
        this->sock.async_send_to(_asio::buffer(send_msg), this->endpoint_list[id], bind);
    }
}

/* フレーム送信時のコールバック */
void UDPFrameSender::onSendFrame(const err_t& err, size_t t_bytes){
    if(err){
        print_err("Failed to send frame", err.message());
    }
    print_debug("ok");
    
    // 全送信が完了したら次番フレームを送信
    const int count = this->send_count.load(std::memory_order_acquire);
    this->send_count.store(count+1, std::memory_order_release);
    if(this->send_count.load(std::memory_order_acquire) == this->display_num){
        this->send_count.store(0, std::memory_order_release);
        this->sendFrame();
    }
}

