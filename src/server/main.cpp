/********************************
 *           main.cpp           *
 *  (ヘッドノード側送信サーバ)  *
 ********************************/

#include "main.hpp"

const int ARGUMENT_NUM = 2;  // コマンドライン引数の個数

/* Main関数 */
int main(int argc, char *argv[]){
    // 設定ファイルをパース
    if(argc != ARGUMENT_NUM){
        std::cerr << "[Error] Invalid arguments." << std::endl;
        std::cerr << "Usage: sbc_server <config file>" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[1]);
    
    // フレーム分割器を初期化
    const char *video_src;
    int row, column;
    std::tie(video_src, row, column) = parser.getVideoSplitterParams();
    VideoSplitter splitter(video_src, row, column);
    
    // 別スレッドで分割フレーム送信器を起動
    smt_ios_t ios = std::make_shared<_asio::io_service>();
    std::vector<std::string> ip_list;
    std::vector<int> port_list;
    int protocol;
    std::tie(ip_list, port_list, protocol) = parser.getFrameSenderParams();
    const int display_num = parser.getDisplayNum();
    std::vector<boost::thread> thread_list(display_num);
    smt_barrier_t barrier = std::make_shared<boost::barrier>(display_num);
    for(int i=0; i<display_num; ++i){
        thread_list[i] = boost::thread(thread_task, ios, splitter.getFrameQueuePtr(i), ip_list[i].c_str(), port_list[i], protocol, barrier);
    }
    
    // フレーム分割器を起動
    splitter.start();
    c_lock(), std::cout << "[Info] Video ended." << std::endl;
    
    // スレッドを終了
    for(int i=0; i<display_num; ++i){
        thread_list[i].join();
    }
    std::cout << "[Info] Terminated threads." << std::endl;
    return 0;
}

/* 別スレッド実行用の関数 */
void thread_task(const smt_ios_t ios, const smt_fq_t queue, const char* const ip, const int port, const int protocol, smt_barrier_t barrier){
    // 分割フレーム送信器を起動
    FrameSender sender(ios, queue, ip, port, protocol, barrier);
    sender.start();
    ios->run();
    return;
}

