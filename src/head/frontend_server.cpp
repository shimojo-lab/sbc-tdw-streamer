/*****************************
*    frontend_server.cpp     *
*   (フロントエンドサーバ)   *
*****************************/

#include "frontend_server.hpp"

/* コンストラクタ */
FrontendServer::FrontendServer(_asio::io_service& ios, ConfigParser& parser, const int port):
    ios(ios),
    acc(ios, _ip::tcp::endpoint(_ip::tcp::v4(), port)),
    fs_port(port)
{
    // パラメータを受け取り
    std::string video_src;
    int column, row, sendbuf_size, sampling_type_, quality_;
    std::tie(video_src, column, row, this->width, this->height, this->stream_port, sendbuf_size,
             this->recvbuf_size, sampling_type_, quality_, this->dec_thre_num, this->ip_addrs
            ) = parser.getFrontendServerParams();
    this->display_num = column * row;
    
    // パラメータを初期化
    this->sock = std::make_shared<_ip::tcp::socket>(ios);
    this->socks = std::vector<sock_ptr_t>(this->display_num);
    this->sampling_type.store(sampling_type_, std::memory_order_release);
    this->quality.store(quality_, std::memory_order_release);
    this->send_bufs = std::vector<jpegbuf_ptr_t>(this->display_num);
    for(int i=0; i<this->display_num; ++i){
        this->send_bufs[i] = std::make_shared<RingBuffer<std::string>>(STATIC_BUF, sendbuf_size);
    }
    
    // 別スレッドでフレーム符号化器を起動
    this->enc_thre = boost::thread(boost::bind(&FrontendServer::runFrameEncoder, this,
                                               video_src, column, row, this->width, this->height)
    );
    
    // 別スレッドでフレーム送信器を起動
    this->send_thre = boost::thread(boost::bind(&FrontendServer::runFrameSender, this));
    
    // 接続待機を開始
    this->waitForConnection();
}

/* 接続待機を開始 */
void FrontendServer::waitForConnection(){
    print_info("Waiting for display node connection at :" + std::to_string(this->fs_port));
    this->acc.async_accept(*this->sock,
                           boost::bind(&FrontendServer::onConnect, this, _ph::error)
    );
}

/* ディスプレイノード接続時のコールバック */
void FrontendServer::onConnect(const err_t& err){
    const std::string ip = this->sock->remote_endpoint().address().to_string();
    if(err){
        print_err("Could not accept " + ip, err.message());
        this->waitForConnection();
        return;
    }
    
    // ディスプレイノードのIDを確認
    const auto iter = std::find(this->ip_addrs.begin(), this->ip_addrs.end(), ip);
    const size_t id = std::distance(this->ip_addrs.begin(), iter);
    if(id == this->ip_addrs.size()){
        print_err(ip + " is not registered", "Check config file");
        this->waitForConnection();
        return;
    }else{
        print_info("Accepted new display node: " + ip);
    }
    
    // 初期化メッセージを返信
    _pt::ptree json;
    std::stringstream jsonstream;
    json.add<int>("width", this->width);
    json.add<int>("height", this->height);
    json.add<int>("stream_port", this->stream_port);
    json.add<int>("recvbuf_size", this->recvbuf_size);
    json.add<int>("dec_thre_num", this->dec_thre_num);
    _pt::write_json(jsonstream, json, false);
    const std::string msg = jsonstream.str() + MSG_DELIMITER;
    _asio::async_write(*this->sock,
                       _asio::buffer(msg),
                       boost::bind(&FrontendServer::onSendInit, this, _ph::error, _ph::bytes_transferred, ip)
    );
    
    // 新規にTCPソケットを用意
    this->socks[id] = this->sock;
    this->sock = std::make_shared<_ip::tcp::socket>(this->ios);
}

/* 初期化メッセージ送信時のコールバック */
void FrontendServer::onSendInit(const err_t& err, size_t t_bytes, const std::string ip){
    if(err){
        print_err("Failed to send init message to " + ip, err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 全ディスプレイノードと接続するまで繰り返し
    ++this->connected_num;
    if(this->connected_num < this->display_num){
        this->waitForConnection();
    }else{
        // 同期制御器を起動
        print_info("All display nodes connected");
        this->sock->close();
        this->runSyncManager();
    }
}

/* 別スレッドでフレーム符号化器を起動 */
void FrontendServer::runFrameEncoder(const std::string video_src, const int column, const int row,
                                     const int width, const int height)
{
    FrameEncoder encoder(video_src, column, row, width, height, this->sampling_type,
                         this->quality, this->send_bufs);
    encoder.run();
}

/* 別スレッドでフレーム送信器を起動 */
void FrontendServer::runFrameSender(){
    _asio::io_service ios;
    FrameSender sender(ios, this->stream_port, this->display_num, this->send_bufs, this->dec_thre_num);
}

/* 同スレッドで同期制御器を起動 */
void FrontendServer::runSyncManager(){
    SyncManager manager(this->ios, this->socks, this->sampling_type, this->quality);
    manager.run();
}
