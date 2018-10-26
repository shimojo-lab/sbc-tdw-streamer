/*****************************************
*               main.cpp                 *
* (ディスプレイノード側表示クライアント) *
******************************************/

#include "main.hpp"

/* 定数の定義 */
const int ARGUMENT_NUM = 2;                       // コマンドライン引数の個数
const char* const APP_NAME = "sbc-tdw-streamer";  // アプリケーション名

/* Main関数 */
int main(int argc, char* argv[]){
    // コマンドライン引数をチェック
    if(argc != ARGUMENT_NUM){
        std::cerr << "[Error] Invaild arguments." << std::endl;
        std::cerr << "Usage: sbc_client <config file>" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    // 設定ファイルをパース
    ConfigParser parser(argv[1]);
    
    // フレーム受信器を起動
    _asio::io_service ios;
    std::shared_ptr<FrameQueue> queue(std::make_shared<FrameQueue>(32));
    int port = parser.getFrameReceiverParams();
    FrameReceiver receiver(ios, queue, port);
    ios.run();
    
    // フレーム表示器を起動
    int res_x, res_y, width, height;
    std::tie(res_x, res_y, width, height) = parser.getFrameViewerParams();
    FrameViewer viewer(APP_NAME, res_x, res_y, width, height, queue);
    viewer.start();
    return 0;
}

