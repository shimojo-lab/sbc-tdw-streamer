/********************************
*     frontend_server.cpp       *
*    (フロントエンドサーバ)     *
********************************/

#include "frontend_server.hpp"

/* コンストラクタ */
FrontendServer::FrontendServer(_ios& ios, ConfigParser& parser):
    ios(ios),
    sock(ios)
{
    // TCPメッセージ受信器を初期化
    const int port = parser.getFrontendServerParams();
    const _ip::tcp::endpoint endpoint(_ip::tcp::v4(), port);
    this->acc.reset(new _ip::tcp::acceptor(ios, endpoint));
    
    // フレーム分割器を初期化
    std::string video_src;
    int row, column;
    std::tie(video_src, row, column) = parser.getVideoSplitterParams();
    this->splitter.reset(new VideoSplitter(video_src, row, column));
    
    // フレーム送信用パラメータを設定
    this->display_num = row * column;
    this->thread_list = std::vector<boost::thread>(display_num);
    this->barrier = std::make_shared<boost::barrier>(this->display_num);
    std::tie(this->protocol, this->sender_port, this->ip_list) = parser.getFrameSenderParams();
    std::tie(this->framerate, this->frame_num) = this->splitter->getVideoParams();
    
    // 接続待機を開始
    print_info("Launched frontend server at :" + std::to_string(port));
    const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
    this->acc->async_accept(this->sock, bind);
}

/* TCP接続時のコールバック */
void FrontendServer::onConnect(const _err& err){
    // 接続元を表示 (失敗時は接続待機を再開)
    const std::string ip = this->sock.remote_endpoint().address().to_string();
    if(err){
        print_err("Failed to accept new display node: " + ip, err.message());
        const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
        this->acc->async_accept(this->sock, bind);
        return;
    }
    print_info("Accepted new display node: " + ip);
    
    // フレーム送信用スレッドを作成
    const auto iter = std::find(this->ip_list.begin(), this->ip_list.end(), ip);
    const std::size_t id = std::distance(this->ip_list.begin(), iter);
    const fq_ptr_t queue = this->splitter->getFrameQueuePtr(id);
    this->thread_list[id] = boost::thread(&FrontendServer::runFrameSender, this, queue);
    
    // 接続元に初期化パラメータを送信
    _pt::ptree init_params;
    std::stringstream jsonstream;
    init_params.add<std::string>("protocol", this->protocol);
    init_params.add<int>("port", this->sender_port);
    init_params.add<int>("framerate", this->framerate);
    init_params.add<int>("frame_num", this->frame_num);
    _pt::write_json(jsonstream, init_params, false);
    std::string bytes_buf = jsonstream.str() + SEPARATOR;
    const auto bind = boost::bind(&FrontendServer::onSendInit, this, _ph::error, _ph::bytes_transferred, ip);
    _asio::async_write(this->sock, _asio::buffer(bytes_buf), bind);
}

/* 初期化メッセージ送信時のコールバック */
void FrontendServer::onSendInit(const _err& err, size_t t_bytes, const std::string ip){
    // 接続数をカウント
    if(err){
        print_err("Failed to send init message to " + ip, err.message());
        return;
    }else{
        ++this->sender_port;
        ++this->connection_num;
    }
    
    if(this->connection_num < this->display_num){
        // 接続待機を再開
        const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
        this->acc->async_accept(this->sock, bind);
    }else{
        // フレーム分割を開始
        this->splitter->run();
    }
}

/* 別スレッドでフレーム送信器を起動 */
void FrontendServer::runFrameSender(const fq_ptr_t queue){
    if(this->protocol == "TCP"){
        TCPFrameSender sender(this->new_ios, queue, this->sender_port, this->barrier);
    }else if(this->protocol == "UDP"){
        //UDPFrameSender sender();
    }else{
        print_err("Invalid protocol is selected", this->protocol);
        return;
    }
    this->new_ios.run();
}

