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
    const char *ip;
    int port;
    const int display_num = parser.getDisplayNum();
    for(int i=0; i<display_num; ++i){
        smt_FrameQueue_t queue = splitter.getFrameQueuePtr(i);
        std::tie(ip, port) = parser.getFrameSenderParams(i);
        std::thread send_thread([&]{
            _asio::io_service ios;
            FrameSender sender(ios, queue);
            ios.run();
            sender.start(ip, port);
        });
        send_thread.detach();
    }
    
    // フレーム分割を開始
    splitter.start();
    return 0;
}

