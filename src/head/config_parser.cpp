/*****************************
*     config_parser.cpp      *
*   (設定ファイルのパーサ)   *
*****************************/

#include "config_parser.hpp"

/* コンストラクタ */
ConfigParser::ConfigParser(const std::string& conf_file):
    BaseConfigParser(conf_file)
{
    // パラメータを取得
    if(!this->readParams(this->conf)){
        std::exit(EXIT_FAILURE);
    }
}

/* パラメータを読み込み */
const bool ConfigParser::readParams(const _pt::ptree& conf){
    // 各種パラメータを取得
    std::string sampling_name;
    try{
        this->video_src = this->getStrParam("video.src");
        this->framerate = this->getIntParam("video.framerate");
        this->column = this->getIntParam("layout.column");
        this->row = this->getIntParam("layout.row");
        this->bezel_w = this->getIntParam("layout.bezel_w");
        this->bezel_h = this->getIntParam("layout.bezel_h");
        this->width = this->getIntParam("resolution.width");
        this->height = this->getIntParam("resolution.height");
        this->fs_port = this->getIntParam("port.frontend_server");
        this->stream_port = this->getIntParam("port.frame_streamer");
        this->sendbuf_size = this->getIntParam("buffer.send_buffer_size");
        this->recvbuf_size = this->getIntParam("buffer.receive_buffer_size");
        sampling_name = this->getStrParam("compression.init_subsampling_type");
        this->quality = this->getIntParam("compression.init_jpeg_quality");
        this->dec_thre_num = this->getIntParam("compression.jpeg_decoder_num");
        this->tuning_term = this->getIntParam("compression.jpeg_tuning_term");
    }catch(...){
        _ml::caution("Could not get parameter", "Config file is invalid");
        return false;
    }
    
    // クロマサブサンプル比を設定
    if(sampling_name=="yuv444"){
        this->sampling_type = TJSAMP_444;
    }else if(sampling_name == "yuv422"){
        this->sampling_type = TJSAMP_422;
    }else if(sampling_name == "yuv420"){
        this->sampling_type = TJSAMP_420;
    }else{
        _ml::caution("Could not get parameter", "chroma subsampling type is invalid");
        return false;
    }
    
    // ディスプレイノードのIPを取得
    for(const auto& elem : conf.get_child("display_node")){
        this->ip_addrs.push_back(elem.second.data());
    }
    if(int(this->ip_addrs.size()) != this->row*this->column){
        _ml::caution("Number of display nodes is invalid", std::to_string(this->ip_addrs.size()));
        return false;
    }
    return true;
}

/* フロントエンドサーバのポート番号を取得 */
const int ConfigParser::getFrontendServerPort(){
    return this->fs_port;
}

/* フロントエンドサーバへ値渡し */
const fs_params_t ConfigParser::getFrontendServerParams(){
    const std::string video_src = this->video_src;
    const int framerate = this->framerate;
    const int column = this->column;
    const int row = this->row;
    const int bezel_w = this->bezel_w;
    const int bezel_h = this->bezel_h;
    const int width = this->width;
    const int height = this->height;
    const int stream_port = this->stream_port;
    const int sendbuf_size = this->sendbuf_size;
    const int recvbuf_size = this->recvbuf_size;
    const int sampling_type = this->sampling_type;
    const int quality = this->quality;
    const int dec_thre_num = this->dec_thre_num;
    const int tuning_term = this->tuning_term;
    const std::vector<std::string> ip_addrs = this->ip_addrs;
    return std::forward_as_tuple(video_src, framerate, column, row, bezel_w, bezel_h, width, height, stream_port,
                                 sendbuf_size, recvbuf_size, sampling_type,
                                 quality, dec_thre_num, tuning_term, ip_addrs);
}

