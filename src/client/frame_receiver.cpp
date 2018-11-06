/*****************************
*     frame_receiver.cpp     *
*    (分割フレーム受信器)    *
******************************/

#include "frame_receiver.hpp"

/* コンストラクタ */
FrameReceiver::FrameReceiver(const ios_ptr_t ios, _asio::io_service::strand& strand, const tcps_ptr_t tcp_sock, const fq_ptr_t queue, const std::string ip, const int port, const std::string protocol):
    ios(ios),
    strand(strand),
    tcp_sock(tcp_sock),
    udp_sock(*ios, _ip::udp::endpoint(_ip::udp::v4(), 30000)),
    queue(queue),
    protocol(protocol)
{
    // TCP接続待機を開始
    print_info("Reconnecting to " + ip + ":" + std::to_string(port));
    const _ip::tcp::endpoint endpoint(_ip::address::from_string(ip), port);
    const auto bind = boost::bind(&FrameReceiver::onConnect, this, _ph::error);
    this->tcp_sock->async_connect(endpoint, this->strand.wrap(bind));
}

/* TCP接続時のコールバック関数 */
void FrameReceiver::onConnect(const _sys::error_code& err){
    if(err){
        print_err("Failed to connect to head node", err.message());
        exit(EXIT_FAILURE);
    }
    print_info("Established TCP connection with head node");
    
    // フレーム受信と同期処理を開始
    if(this->protocol == "TCP"){
        const auto bind = boost::bind(&FrameReceiver::onRecvByTCP, this, _ph::error, _ph::bytes_transferred);
        _asio::async_read_until(*this->tcp_sock, this->tcp_recv_buf, SEPARATOR, this->strand.wrap(bind));
    }else if(this->protocol == "UDP"){
        const _ip::udp::endpoint endpoint;
        const auto bind = boost::bind(&FrameReceiver::onRecvByUDP, this, _ph::error, _ph::bytes_transferred);
        //this->udp_sock.async_receive_from(_asio::buffer(this->udp_recv_buf), endpoint, bind);
    }
    return;
}

/* TCPでのフレーム受信時のコールバック */
void FrameReceiver::onRecvByTCP(const _sys::error_code& err, std::size_t t_bytes){
    if(err){
        print_err("Failed to receive frame", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // フレームを取得
    const auto data = this->tcp_recv_buf.data();
    std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<SEPARATOR_LEN; ++i){
        bytes_buf.pop_back();
    }
    std::vector<unsigned char> comp_buf(bytes_buf.c_str(), bytes_buf.c_str()+bytes_buf.length());
    this->queue->enqueue(comp_buf);
    this->tcp_recv_buf.consume(t_bytes);
    
    // フレーム受信を再開
    const auto bind = boost::bind(&FrameReceiver::onRecvByTCP, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(*this->tcp_sock, this->tcp_recv_buf, SEPARATOR, this->strand.wrap(bind));
    return;
}

/* UDPでのフレーム受信時のコールバック */
void FrameReceiver::onRecvByUDP(const _sys::error_code& err, std::size_t t_bytes){
    if(err){
        print_err("Failed to receive frame", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 受信用バッファからフレームのデータを抽出
/*    const auto data = this->udp_recv_buf.data();
    std::string bytes_buf(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    for(int i=0; i<SEPARATOR_LEN; ++i){
        bytes_buf.pop_back();
    }
    std::vector<unsigned char> comp_buf(bytes_buf.c_str(), bytes_buf.c_str()+bytes_buf.length());
    this->queue->enqueue(comp_buf);
    this->udp_recv_buf.consume(t_bytes);
    
    // フレーム受信を再開
    const auto bind = boost::bind(&FrameReceiver::onRecvByUDP, this, _ph::error, _ph::bytes_transferred);
    this->udp_sock.async_receive_from(_asio::buffer(this->udp_recv_buf), endpoint, bind);
*/    return;
}

