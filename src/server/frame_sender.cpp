/*************************
*    frame_sender.cpp    *
*    (フレーム送信器)    *
*************************/

#include "frame_sender.hpp"

/* コンストラクタ */
FrameSender::FrameSender(const ios_ptr_t ios, _asio::io_service::strand& strand, const fq_ptr_t queue, const int port, const std::string protocol, bar_ptr_t barrier):
    ios(ios),
    strand(strand),
    tcp_sock(*ios),
    udp_sock(*ios, _ip::udp::endpoint(_ip::udp::v4(), port)),
    acc(*ios, _ip::tcp::endpoint(_ip::tcp::v4(), port)),
    queue(queue),
    udp_port(port),
    protocol(protocol),
    barrier(barrier)
{
    // 接続待機を開始
    print_info("Launched streaming server at :" + std::to_string(port));
    const auto bind = boost::bind(&FrameSender::onConnect, this, _ph::error);
    this->acc.async_accept(this->tcp_sock, this->strand.wrap(bind));
}

/* TCP接続時のコールバック */
void FrameSender::onConnect(const _sys::error_code& err){
    // 接続元を表示
    const std::string ip = this->tcp_sock.remote_endpoint().address().to_string();
    if(err){
        print_err("Failed to establish TCP connection with " + ip, err.message());
        return;
    }
    print_info("Established TCP connection with " + ip);
    
    // フレーム送信を開始
    const cv::Mat frame = this->queue->dequeue();
    if(this->protocol == "TCP"){
        const auto bind = boost::bind(&FrameSender::onSendFrameByTCP, this, _ph::error, _ph::bytes_transferred);
        _asio::async_write(this->tcp_sock, _asio::buffer(this->compressFrame(frame)), this->strand.wrap(bind));
    }else if(this->protocol == "UDP"){
        const auto bind = boost::bind(&FrameSender::onSendFrameByUDP, this, _ph::error, _ph::bytes_transferred);
        this->udp_endpoint = _ip::udp::endpoint(_ip::address::from_string(ip), this->udp_port);
        this->udp_sock.async_send_to(_asio::buffer(this->compressFrame(frame)), this->udp_endpoint, bind);
    }
    
    // 同期メッセージ受信を開始
    const auto bind = boost::bind(&FrameSender::onRecvSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->tcp_sock, this->recv_buf, SEPARATOR, this->strand.wrap(bind));
    return;
}

/* TCPでのフレーム送信時のコールバック */
void FrameSender::onSendFrameByTCP(const _sys::error_code& err, std::size_t t_bytes){
    if(err){
        print_err("Failed to send frame", err.message());
    }
    
    // 次番のフレームを送信
    const cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&FrameSender::onSendFrameByTCP, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->tcp_sock, _asio::buffer(this->compressFrame(frame)), bind);
    return;
}

/* UDPでの分割フレーム送信時のコールバック */
void FrameSender::onSendFrameByUDP(const _sys::error_code& err, std::size_t t_bytes){
    if(err){
        print_err("Failed to send frame", err.message());
    }
    
    // 次番のフレームを送信
    const cv::Mat frame = this->queue->dequeue();
    const auto bind = boost::bind(&FrameSender::onSendFrameByUDP, this, _ph::error, _ph::bytes_transferred);
    this->udp_sock.async_send_to(_asio::buffer(this->compressFrame(frame)), this->udp_endpoint, bind);
    return;
}

/* 同期メッセージ受信時のコールバック */
void FrameSender::onRecvSync(const _sys::error_code& err, std::size_t t_bytes){
    // 全ディスプレイノード間で同期
    this->barrier->wait();
    
    // 同期メッセージを送信
    std::string bytes_buf("sync");
    bytes_buf += SEPARATOR;
    const auto bind = boost::bind(&FrameSender::onSendSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_write(this->tcp_sock, _asio::buffer(bytes_buf), bind);
    return;
}

/* 同期メッセージ送信時のコールバック */
void FrameSender::onSendSync(const _sys::error_code& err, std::size_t t_bytes){
    // 同期メッセージの受信を再開
    const auto bind = boost::bind(&FrameSender::onRecvSync, this, _ph::error, _ph::bytes_transferred);
    _asio::async_read_until(this->tcp_sock, this->recv_buf, SEPARATOR, bind);
    return;
}

/* フレームを圧縮 */
std::string FrameSender::compressFrame(const cv::Mat& frame){
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 90};
    cv::imencode(".jpg", frame, this->comp_buf, params);
    std::string bytes_buf(this->comp_buf.begin(), this->comp_buf.end());
    bytes_buf += SEPARATOR;
    return bytes_buf;
}

