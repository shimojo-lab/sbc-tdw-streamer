/********************************
 *           main.cpp           *
 *  (ヘッドノード側プログラム)  *
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
    
    // 設定読み込みモジュールを起動
    ConfigParser parser(argv[1]);
    
    // フレーム分割モジュールを起動
    const char *filename;
    int row;
    int column;
    std::tie(filename, row, column) = parser.getVideoDemuxerParams();
    VideoDemuxer demuxer(filename, row, column);
    
    // フレーム送信スレッドを起動
    const int display_num = parser.getDisplayNum();
    const char *ip;
    int port;
    for(int i=0; i<display_num; ++i){
        std::tie(ip, port) = parser.getFrameStreamerParams(i);
        std::thread send_thread([&demuxer, &ip, &port, &i]{
            _asio::io_service ios;
            FrameStreamer streamer(ios, demuxer, ip, port, i);
            ios.run();
        });
        send_thread.detach();
    }
    while(true){}
    return 0;
}

