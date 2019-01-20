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
    int framerate, column, row, bezel_w, bezel_h, width, height, stream_port, sendbuf_num, recvbuf_num, sampling_type_, quality_, dec_thre_num, tuning_term;
    std::tie(
        video_src, framerate, column, row, bezel_w, bezel_h, width, height, stream_port,
        sendbuf_num, recvbuf_num, sampling_type_, quality_, dec_thre_num,
        tuning_term, this->ip_addrs
    ) = parser.getFrontendServerParams();
    this->display_num = column * row;
    
    // 初期化メッセージ用パラメータを設定
    this->init_params.setParam("width", width);
    this->init_params.setParam("height", height);
    this->init_params.setParam("stream_port", stream_port);
    this->init_params.setParam("framerate", framerate);
    this->init_params.setParam("recvbuf_num", recvbuf_num);
    this->init_params.setParam("dec_thre_num", dec_thre_num);
    this->init_params.setParam("tuning_term", tuning_term);
    
    // パラメータを初期化
    this->sock = std::make_shared<_ip::tcp::socket>(ios);
    this->socks = std::vector<sock_ptr_t>(this->display_num);
    this->sampling_type.store(sampling_type_, std::memory_order_release);
    this->quality.store(quality_, std::memory_order_release);
    this->send_bufs = std::vector<tranbuf_ptr_t>(this->display_num);
    for(int i=0; i<this->display_num; ++i){
        this->send_bufs[i] = std::make_shared<TransceiveFramebuffer>(sendbuf_num);
    }
    
    // 別スレッドでフレーム圧縮器を起動
    this->enc_thre = boost::thread(boost::bind(&FrontendServer::runFrameEncoder,
                                               this,
                                               video_src,
                                               column,
                                               row,
                                               bezel_w,
                                               bezel_h,
                                               width,
                                               height)
    );
    
    // 別スレッドでフレーム送信器を起動
    this->send_thre = boost::thread(boost::bind(&FrontendServer::runFrameSender,
                                                this,
                                                stream_port,
                                                dec_thre_num+VIEWBUF_EXTRA_NUM)
    );
    
    // 接続待機を開始
    this->waitForConnection();
}

/* 接続待機を開始 */
void FrontendServer::waitForConnection(){
    _ml::notice("Waiting for display node connection at :" + std::to_string(this->fs_port));
    this->acc.async_accept(*this->sock,
                           boost::bind(&FrontendServer::onConnect, this, _ph::error)
    );
}

/* ディスプレイノード接続時のコールバック */
void FrontendServer::onConnect(const err_t& err){
    const std::string ip = this->sock->remote_endpoint().address().to_string();
    if(err){
        _ml::caution("Could not accept " + ip, err.message());
        std::exit(EXIT_FAILURE);
        return;
    }
    
    // ディスプレイノードのIDを確認
    const auto iter = std::find(this->ip_addrs.begin(), this->ip_addrs.end(), ip);
    const size_t id = std::distance(this->ip_addrs.begin(), iter);
    if(id == this->ip_addrs.size()){
        _ml::caution(ip + " is not registered", "Check config file");
        std::exit(EXIT_FAILURE);
        return;
    }else{
        _ml::notice("Accepted new display node: " + ip);
    }
    
    // 初期化メッセージを返信
    const std::string msg = this->init_params.serialize() + MSG_DELIMITER;
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
        _ml::caution("Failed to send init message to " + ip, err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 全ディスプレイノードと接続するまで繰り返し
    ++this->connected_num;
    if(this->connected_num < this->display_num){
        this->waitForConnection();
    }else{
        // 同スレッドで同期制御器を起動
        _ml::notice("All display nodes connected");
        this->sock->close();
        this->runSyncManager();
    }
}

/* 別スレッドでフレーム符号化器を起動 */
void FrontendServer::runFrameEncoder(const std::string video_src, const int column, const int row,
                                     const int bezel_w, const int bezel_h, const int width, const int height)
{
    FrameEncoder encoder(video_src,
                         column,
                         row,
                         bezel_w,
                         bezel_h,
                         width,
                         height,
                         this->sampling_type,
                         this->quality,
                         this->send_bufs
    );
    encoder.run();
}

/* 別スレッドでフレーム送信器を起動 */
void FrontendServer::runFrameSender(const int stream_port, const int viewbuf_num){
    _asio::io_service ios;
    FrameSender sender(ios,
                       stream_port,
                       this->display_num,
                       this->send_bufs,
                       viewbuf_num
    );
}

/* 同スレッドで同期制御器を起動 */
void FrontendServer::runSyncManager(){
    SyncManager manager(this->ios,
                        this->socks,
                        this->sampling_type,
                        this->quality
    );
    manager.run();
}

