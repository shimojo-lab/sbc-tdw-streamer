/********************************
*     frontend_server.cpp       *
*    (フロントエンドサーバ)     *
********************************/

#include "frontend_server.hpp"

/* コンストラクタ */
FrontendServer::FrontendServer(const ios_ptr_t& ios, ConfigParser& parser):
    ios(ios),
    sock(*ios)
{
    // TCPメッセージ受信器を初期化
    int port = parser.getFrontendServerParams();
    _ip::tcp::endpoint endpoint(_ip::tcp::v4(), port);
    this->acc.reset(new _ip::tcp::acceptor(*ios, endpoint));
    
    // フレーム分割器を初期化
    const char *video_src;
    int row, column;
    std::tie(video_src, row, column) = parser.getVideoSplitterParams();
    this->splitter.reset(new VideoSplitter(video_src, row, column));
    
    // フレーム送信用パラメータを設定
    this->display_num = row * column;
    this->thread_list = std::vector<boost::thread>(display_num);
    std::tie(this->protocol, this->sender_port, this->ip_list) = parser.getFrameSenderParams();
    std::tie(this->framerate, this->frame_num) = this->splitter->getVideoParams();
    
    // 接続待機を開始
    print_info("Lauinched frontend server at :" + std::to_string(port));
    const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
    this->acc->async_accept(this->sock, bind);
}

/* TCP接続時のコールバック */
void FrontendServer::onConnect(const _sys::error_code& err){
    // 接続元を表示 (失敗時は接続待機を再開)
    std::string ip = this->sock.remote_endpoint().address().to_string();
    if(err){
        print_err("Failed to accept new display node: " + ip, err.message());
        const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
        this->acc->async_accept(this->sock, bind);
        return;
    }
    print_info("Accepted new display node: " + ip);
    
    // フレーム送信用スレッドを作成
    const std::vector<std::string>::iterator iter = std::find(this->ip_list.begin(), ip_list.end(), ip);
    const std::size_t id = std::distance(this->ip_list.begin(), iter);
    const fq_ptr_t queue = this->splitter->getFrameQueuePtr(id);
    this->thread_list[id] = boost::thread(&FrontendServer::run_frame_sender, this, queue);
    
    // 接続元に初期化パラメータを送信
    _pt::ptree init_params;
    std::stringstream jsonstream;
    init_params.add<int>("id", id);
    init_params.add<std::string>("protocol", this->protocol);
    init_params.add<int>("port", this->sender_port);
    init_params.add<int>("framerate", this->framerate);
    init_params.add<int>("total_frame_num", this->frame_num);
    _pt::write_json(jsonstream, init_params, false);
    const auto bind = boost::bind(&FrontendServer::onSendInit, this, _ph::error, _ph::bytes_transferred, ip, id);
    _asio::async_write(this->sock, _asio::buffer(jsonstream.str()), bind);
    ++this->sender_port;
    return;
}

/* 初期化メッセージ送信時のコールバック */
void FrontendServer::onSendInit(const _sys::error_code &err, std::size_t t_bytes, std::string ip, std::size_t id){
    // 接続数をカウント
    if(err){
        print_err("Failed to send init message to " + ip, err.message());
        this->thread_list[id].interrupt();
        this->thread_list[id].join();
    }else{
        ++this->connection_num;
    }
    
    if(this->connection_num < this->display_num){
        // 接続待機を再開
        const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
        this->acc->async_accept(this->sock, bind);
    }else{
        // フレーム分割を開始
        this->splitter->start();
    }
    return;
}

/* 別スレッドでフレーム送信器を起動 */
void FrontendServer::run_frame_sender(const fq_ptr_t queue){
    FrameSender sender(this->ios, queue, this->sender_port, this->protocol);
    this->ios->run();
    return;
}

