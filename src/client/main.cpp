/*****************************************
*               main.cpp                 *
* (ディスプレイノード側表示クライアント) *
******************************************/

#include "main.hpp"

const int ARGUMENT_NUM = 2;                       // コマンドライン引数の個数
const char* const APP_NAME = "sbc-tdw-streamer";  // アプリケーション名

/* Main関数 */
int main(int argc, char* argv[]){
    // 設定ファイルをパース
    if(argc != ARGUMENT_NUM){
        std::cerr << "[Error] Invaild arguments." << std::endl;
        std::cerr << "Usage: sbc_client <config file>" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    ConfigParser parser(argv[1]);
    
    // 分割フレーム表示器を起動
    int res_x, res_y, width, height;
    std::tie(res_x, res_y, width, height) = parser.getFrameViewerParams();
    smt_fq_t queue = std::make_shared<FrameQueue>(64);
    FrameViewer viewer(APP_NAME, res_x, res_y, width, height, queue);
    
    // 別スレッドで分割フレーム受信器を起動
    smt_ios_t ios = std::make_shared<_asio::io_service>();
    int port = parser.getFrameReceiverParams();
    boost::thread receive_thread(thread_task, ios, queue, port);
    
    // 分割フレームの表示を開始
    viewer.start();
    std::cout << "[Info] Ended play video." << std::endl;
    
    // スレッドを終了
    receive_thread.join();
    std::cout << "[Info] Teminated threads." << std::endl;
    return 0;
}

/* 別スレッド実行用の関数 */
void thread_task(const smt_ios_t ios, const smt_fq_t queue, const int port){
    // 分割フレーム受信器を起動
    FrameReceiver receiver(ios, queue, port);
    ios->run();
    return;
}

