/*******************************
*     frontend_server.cpp      *
*    (フロントエンドサーバ)    *
*******************************/

#include "frontend_server.hpp"

/* コンストラクタ */
FrontendServer::FrontendServer(ios_t& ios, ConfigParser& parser):
    ios(ios),
    acc(ios, tcp_t::endpoint(tcp_t::v4(), parser.getFrontendPort()))
{
    // TCPソケットを初期化
    this->sock = std::make_shared<tcp_t::socket>(ios);
    
    // フレーム分割器を初期化
    std::string video_src;
    int row, column;
    std::tie(video_src, row, column) = parser.getVideoSplitterParams();
    this->splitter = std::make_shared<VideoSplitter>(video_src, row, column);
    
    // フレーム送信用パラメータを設定
    this->display_num = row * column;
    this->sock_list = std::vector<tcps_ptr_t>(display_num);
    this->sender_thre = std::vector<boost::thread>(display_num);
    std::tie(this->protocol, this->sender_port, this->ip_list) = parser.getFrameSenderParams();
    std::tie(this->frame_num, this->frame_rate) = this->splitter->getVideoParams();
    
    // 接続待機を開始
    const int port = parser.getFrontendPort();
    print_info("Launched frontend server at :" + std::to_string(port));
    const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
    this->acc.async_accept(*this->sock, bind);
}

/* TCP接続時のコールバック */
void FrontendServer::onConnect(const err_t& err){
    // 接続元を表示 (失敗時は接続待機を再開)
    const std::string ip = this->sock->remote_endpoint().address().to_string();
    if(err){
        print_err("Failed to accept new display node: " + ip, err.message());
        return;
    }
    print_info("Accepted new display node: " + ip);
    
    // ディスプレイノードのIDを取得
    //const auto iter = std::find(this->ip_list.begin(), this->ip_list.end(), ip);
    //const int id = std::distance(this->ip_list.begin(), iter);
    
    // 新規TCPソケットを用意
    this->sock_list[id] = this->sock;
    this->sock = std::make_shared<tcp_t::socket>(this->ios);
    
    // フレーム送信スレッドを作成
    const fq_ptr_t queue = this->splitter->getFrameQueuePtr(id);
    const auto sender_bind = boost::bind(&FrontendServer::runFrameSender, this, queue);
    this->sender_thre[id] = boost::thread(sender_bind);
    
    // 接続元に初期化パラメータを送信
    _pt::ptree init_params;
    std::stringstream jsonstream;
    init_params.add<std::string>("protocol", this->protocol);
    init_params.add<int>("port", this->sender_port);
    init_params.add<int>("frame_num", this->frame_num);
    init_params.add<double>("frame_rate", this->frame_rate);
    _pt::write_json(jsonstream, init_params, false);
    std::string bytes_buf = jsonstream.str() + SEPARATOR;
    const auto init_bind = boost::bind(&FrontendServer::onSendInit, this, _ph::error, _ph::bytes_transferred, ip);
    _asio::async_write(*this->sock_list[id], _asio::buffer(bytes_buf), init_bind);
this->id++;
}

/* 初期化メッセージ送信時のコールバック */
void FrontendServer::onSendInit(const err_t& err, size_t t_bytes, const std::string ip){
    // 接続数をカウント
    if(err){
        print_err("Failed to send init message to " + ip, err.message());
        return;
    }else{
        ++this->sender_port;
        ++this->connection_num;
    }
    
    if(this->connection_num != this->display_num){
        // 接続待機を再開
        const auto bind = boost::bind(&FrontendServer::onConnect, this, _ph::error);
        this->acc.async_accept(*this->sock, bind);
    }else{
        // 接続待機を終了
        print_info("Accepted all display nodes");
        this->sock->close();
        
        // フレーム分割を開始
        const auto bind = boost::bind(&FrontendServer::runVideoSplitter, this, this->splitter);
        this->splitter_thre = boost::thread(bind);
        
        // 表示タイミング制御を開始
        print_info("Start playback");
        this->runViewerSynchronizer();
    }
}

/* 別スレッドでフレーム送信器を起動 */
void FrontendServer::runFrameSender(const fq_ptr_t queue){
    ios_t ios;
    if(this->protocol == "TCP"){
        TCPFrameSender sender(ios, queue, this->sender_port);
    }else if(this->protocol == "UDP"){
        //UDPFrameSender sender(ios, queue, this->sender_port);
    }else{
        print_err("Invalid protocol is selected", this->protocol);
        return;
    }
}

/* 別スレッドでフレーム分割器を起動 */
void FrontendServer::runVideoSplitter(const vs_ptr_t splitter){
    print_info("Launched video splitter");
    splitter->run();
}

/* 表示タイミング制御器を起動 */
void FrontendServer::runViewerSynchronizer(){
    print_info("Launched viewer synchronizer");
    ViewerSynchronizer synchronizer(this->ios, this->sock_list);
    synchronizer.run();
    this->ios.run();
}

