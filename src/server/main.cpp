/** main.cpp **/

#include "main.hpp"

/* Main関数 */
int main(int argc, char *argv[]){
/*    // 設定ファイルをパース
    pt::ptree conf;
    pt::read_json("config.json", conf);
    boost::optional<std::string> filename = conf.get_optional<std::string>("head_node.filename");
    boost::optional<int> x = conf.get_optional<int>("head_node.layout.x");
    boost::optional<int> y = conf.get_optional<int>("head_node.layout.y");
    boost::optional<int> width = conf.get_optional<int>("head_node.resolution.width");
    boost::optional<int> height = conf.get_optional<int>("head_node.resoution.height");
    pt::ptree display_node = conf.get_child("display_node");
    
    // 動画フレーム分割器を起動
    VideoDemuxer demuxer(filename->c_str(), *x, *y);
*/    
    // フレーム送信スレッドを起動
    std::vector<std::thread> threads;
    asio::io_service ios;
    FrameStreamer streamer(&ios, "127.0.0.1", 11111);
    ios.run();
    return 0;
}

