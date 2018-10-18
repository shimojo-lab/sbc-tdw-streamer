/*******************************
 *           main.cpp          *
 *      (表示側プログラム)     *
 *******************************/

#include "main.hpp"

/* Main関数 */
int main(int argc, char* argv[]){
    // コマンドライン引数をチェック
    if(argc != ARGUMENT_NUM){
        std::cerr << "[Error] Invaild arguments." << std::endl;
        std::cerr << "Usage: ./client <config_file>" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    // 設定読み込みモジュールを起動
    ConfigParser parser(argv[1]);
    
    // フレーム表示モジュールを起動
    int res_width;
    int res_height;
    std::tie(res_width, res_height) = parser.getFrameViewerParams();
    FrameViewer viewer(res_height, res_height);
    
    // フレーム受信モジュールを起動
    _asio::io_service ios;
    int port = parser.getFrameReceiverParams();
    FrameReceiver receiver(ios, viewer, port);
    ios.run();
    
    return 0;
}

