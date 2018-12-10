/*******************************
*     frontend_server.cpp      *
*    (フロントエンドサーバ)    *
*******************************/

#include "frontend_server.hpp"

/* コンストラクタ */
FrontendServer::FrontendServer(ios_t& ios, ConfigParser& parser, const int frontend_port):
    ios(ios),
    acc(ios, tcp_t::endpoint(tcp_t::v4(), frontend_port))
{
    // パラメータを初期化
    int sbuf_size, init_quality;
    std::tie(this->video_src, this->row, this->column, this->sender_port, this->protocol_type, sbuf_size, init_quality, this->ip_list) = parser.getFrontendServerParams();
    this->sock = std::make_shared<tcp_t::socket>(ios);
    this->sock_list = std::vector<tcps_ptr_t>(this->row * this->column);
    this->sbuf = std::make_shared<RingBuffer<std::string>>(STATIC_BUF, sbuf_size);
    this->comp_quality.store(init_quality, std::memory_order_release);
    this->send_semaphore.store(true, std::memory_order_release);
    
    // 別スレッドでフレーム符号化器を起動
    this->enc_thre = boost::thread(boost::bind(&FrontendServer::runFrameEncoder, this));
    
    // 別スレッドでフレーム送信器を起動
    this->send_thre = boost::thread(boost::bind(&FrontendServer::runFrameSender, this));
    
    // 接続待機を開始
    print_info("Launched frontend server at :" + std::to_string(frontend_port));
    const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
    this->acc.async_accept(*this->sock, bind);
}

/* ディスプレイノード接続時のコールバック */
void FrontendServer::onConnect(const err_t& err){
    // 接続元を表示
    const std::string ip = this->sock->remote_endpoint().address().to_string();
    if(err){
        print_err("Failed to accept new display node: " + ip, err.message());
        const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
        this->acc.async_accept(*this->sock, bind);
        return;
    }
    print_info("Accepted new display node: " + ip);
    
    // 接続元のIDを取得
    const auto iter = std::find(this->ip_list.begin(), this->ip_list.end(), ip);
    const int id = std::distance(this->ip_list.begin(), iter);
    
    // 接続元に初期化メッセージを送信
    _pt::ptree init_params;
    std::stringstream jsonstream;
    init_params.add<int>("id", id);
    init_params.add<int>("row", this->row);
    init_params.add<int>("column", this->column);
    init_params.add<int>("width", this->width);
    init_params.add<int>("height", this->height); 
    init_params.add<int>("port", this->sender_port);
    init_params.add<int>("protocol_type", this->protocol_type);
    _pt::write_json(jsonstream, init_params, false);
    std::string bytes_buf = jsonstream.str() + MSG_DELIMITER;
    const auto bind = boost::bind(&FrontendServer::onSendInit, this, _ph::error, _ph::bytes_transferred, ip);
    _asio::async_write(*this->sock, _asio::buffer(bytes_buf), bind);
    
    // 新規TCPソケットを用意
    this->sock_list[id] = this->sock;
    this->sock = std::make_shared<tcp_t::socket>(this->ios);
}

/* 初期化メッセージ送信時のコールバック */
void FrontendServer::onSendInit(const err_t& err, size_t t_bytes, const std::string ip){
    // 接続数をカウント
    if(err){
        print_err("Failed to send init message to " + ip, err.message());
    }else{
        ++this->connection_num;
    }
    
    if(this->connection_num < this->row*this->column){
        // 接続待機を再開
        const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
        this->acc.async_accept(*this->sock, bind);
    }else{
        // 接続待機を終了
        print_info("Finished to accept all display nodes");
        this->sock->close();
        
        // 同スレッドで同期制御を開始
        print_info("Start playback video");
        this->runViewerSynchronizer();
    }
}

/* 別スレッドでフレーム送信器を起動 */
void FrontendServer::runFrameSender(){
    ios_t ios;
    if(this->protocol_type == 0){
        TCPFrameSender sender(ios, this->sbuf, this->sender_port, this->row*this->column, this->send_semaphore);
    }else if(this->protocol_type == 1){
        UDPFrameSender sender(ios, this->sbuf, this->sender_port, this->row*this->column, this->ip_list, this->send_semaphore);
    }else{
        print_err("Invalid protocol is selected", "Select from TCP or UDP");
    }
}

/* 別スレッドでフレーム符号化器を起動 */
void FrontendServer::runFrameEncoder(){
    FrameEncoder encoder(this->video_src, this->sbuf, INIT_CBUF_SIZE, this->comp_quality);
    std::tie(this->width, this->height) = encoder.getFrameSize();
    encoder.run();
}

/* 同スレッドで同期制御器を起動 */
void FrontendServer::runViewerSynchronizer(){
    ViewerSynchronizer synchronizer(this->ios, this->sock_list, this->comp_quality, this->send_semaphore);
    synchronizer.run();
}

