/********************************
 *           main.cpp           *
 *  (ヘッドノード側送信サーバ)  *
 ********************************/

#include "main.hpp"

/* 定数の定義 */
const int ARGUMENT_NUM = 2;  // コマンドライン引数の個数

/* Main関数 */
int main(int argc, char *argv[]){
    // コマンドライン引数をチェック
    if(argc != ARGUMENT_NUM){
        std::cerr << "[Error] Invalid arguments." << std::endl;
        std::cerr << "Usage: sbc_server <config file>" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    // 設定ファイルをパース
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
    std::tie(ip_list, port_list) = parser.getFrameSenderParams();
    const int display_num = parser.getDisplayNum();
    std::vector<boost::thread> thread_list(display_num);
    for(int i=0; i<display_num; ++i){
        thread_list[i] = boost::thread(thread_task,
                                       ios, splitter.getFrameQueuePtr(i),
                                       ip_list[i].c_str(), port_list[i]);
    }
    
    // フレーム分割を開始
    splitter.start();
    
    // スレッドを終了
    for(int i=0; i<display_num; ++i){
        thread_list[i].join();
    }
    return 0;
}

/* 別スレッド実行用の関数 */
void thread_task(smt_ios_t ios, smt_fq_t queue, const char *ip, int port){
    // 分割フレーム送信器を初期化
    FrameSender sender(ios, queue);
    sender.start(ip, port);
    
    // 送信処理を開始
    ios->run();
    return;
}

