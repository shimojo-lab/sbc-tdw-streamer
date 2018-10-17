/********************************
 *           main.cpp           *
 *      (送信側プログラム)      *
 ********************************/

#include "main.hpp"

/* Main関数 */
int main(int argc, char *argv[]){
    // コマンドライン引数をチェック
    if(argc != ARGUMENT_NUM){
        std::cerr << "[Error] Invalid arguments." << std::endl;
        std::cerr << "Usage: server ＜Config File＞" << std::endl;
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
    
    // フレーム送信モジュールを起動
    int display_num = parser.getDisplayNum();
    std::vector<std::thread> threads;
    for(int i=0; i<display_num; ++i){
        _asio::io_service ios;
        const char *ip;
        int port;
        std::tie(ip, port) = parser.getFrameStreamerParams(i);
        FrameStreamer streamer(ios, demuxer, ip, port);
        ios.run();
    }
    return 0;
}

