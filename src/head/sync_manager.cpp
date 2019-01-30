/*************************
*    sync_manager.cpp    *
*      (同期制御器)      *
*************************/

#include "sync_manager.hpp"

/* コンストラクタ */
SyncManager::SyncManager(_asio::io_service& ios, std::vector<sock_ptr_t>& socks,
                         const int target_fps, jpeg_params_t& yuv_format_list,
                         jpeg_params_t& quality_list):
    ios(ios),
    socks(socks),
    display_num(socks.size()),
    target_fps(target_fps),
    sync_count(0),
    yuv_format_list(yuv_format_list),
    quality_list(quality_list)
{
    // パラメータを初期化
    for(int i=0; i<this->display_num; ++i){
        this->stream_bufs.push_back(std::make_shared<_asio::streambuf>());
    }
}

/* YUVサンプル比を変更 */
const std::string SyncManager::tuneYUV(const int change_flag, const int id){
    std::string new_type = "";
    switch(change_flag){
    case JPEG_PARAM_UP:
        switch(this->yuv_format_list[id].load(std::memory_order_acquire)){
        case TJSAMP_422:
            this->yuv_format_list[id].store(TJSAMP_444, std::memory_order_release);
            new_type = "yuv444";
            break;
        case TJSAMP_420:
            this->yuv_format_list[id].store(TJSAMP_422, std::memory_order_release);
            new_type = "yuv422";
            break;
        }
        break;
    case JPEG_PARAM_DOWN:
        switch(this->yuv_format_list[id].load(std::memory_order_acquire)){
        case TJSAMP_444:
            this->yuv_format_list[id].store(TJSAMP_422, std::memory_order_release);
            new_type = "yuv422";
            break;
        case TJSAMP_422:
            this->yuv_format_list[id].store(TJSAMP_420, std::memory_order_release);
            new_type = "yuv420";
            break;
        }
        break;
    }
    return new_type;
}

/* 品質係数を変更 */
const std::string SyncManager::tuneQuality(const int change_flag, const int id){
    int new_quality;
    switch(change_flag){
    case JPEG_PARAM_UP:
        if(this->quality_list[id].load(std::memory_order_acquire) < JPEG_QUALITY_MAX){
            new_quality = this->quality_list[id].fetch_add(1, std::memory_order_release) + 1;
        }
        break;
    case JPEG_PARAM_DOWN:
        if(this->quality_list[id].load(std::memory_order_acquire) > JPEG_QUALITY_MIN){
            new_quality = this->quality_list[id].fetch_sub(1, std::memory_order_release) - 1;
        }
        break;
    }
    return std::to_string(new_quality);
}

/* 同期メッセージをパース */
void SyncManager::parseSyncMsg(const std::string& sync_msg, const int id){
    this->sync_params.deserialize(sync_msg);
    const int param_flag = this->sync_params.getIntParam("param");
    const int change_flag = this->sync_params.getIntParam("change");
    
    if(param_flag == JPEG_YUV_CHANGE){  // YUVサンプル比を変更 
        const std::string new_type = this->tuneYUV(change_flag, id);
        if(new_type != ""){
            _ml::notice("Display"+std::to_string(id)+": "+"YUV format is changed to "+new_type);
        }
    }else if(param_flag == JPEG_QUALITY_CHANGE){  // 品質係数を変更
        std::string new_quality = this->tuneQuality(change_flag, id);
        if(new_quality != ""){
            _ml::notice("Display"+std::to_string(id)+": "+"Quality is changed to "+new_quality);
        }
    }
}

/* 同期メッセージ受信時のコールバック */
void SyncManager::onRecvSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        _ml::caution("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージをパース
    const auto data = this->stream_bufs[id]->data();
    std::string sync_msg(_asio::buffers_begin(data), _asio::buffers_end(data));
    sync_msg.erase(sync_msg.length()-MSG_DELIMITER_LEN);
    this->parseSyncMsg(sync_msg, id);
    this->stream_bufs[id]->consume(t_bytes);
    
    // 全ディスプレイノード間で同期
    this->sync_count.fetch_add(1, std::memory_order_release);
    if(this->sync_count.load(std::memory_order_acquire) == this->display_num){
        // 1秒ごとにフレームレートを算出
        bool dec_speed_down = false;
        ++this->frame_count;
        const hr_clock_t post_t = _chrono::high_resolution_clock::now();
        this->elapsed_t += _chrono::duration_cast<_chrono::milliseconds>(post_t-this->pre_t).count();
        if(this->elapsed_t > 1000.0){
            ++this->total_sec;
            _ml::notice(std::to_string(this->total_sec) + "s: " +
                        std::to_string(this->frame_count) + "fps"
            );
            dec_speed_down = this->frame_count > this->target_fps;
            this->elapsed_t -= 1000.0;
            this->frame_count = 0;
        }
        this->pre_t = post_t;
        this->sync_count.store(0, std::memory_order_release);
        this->sendSync(dec_speed_down);
    }
}

/* 同期メッセージ送信時のコールバック */
void SyncManager::onSendSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        _ml::caution("Failed to send sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // 同期メッセージ受信を再開
    _asio::async_read_until(*this->socks[id],
                            *this->stream_bufs[id],
                            MSG_DELIMITER,
                            boost::bind(&SyncManager::onRecvSync, this, _ph::error, _ph::bytes_transferred, id)
    );
}

/* 同期メッセージを送信 */
void SyncManager::sendSync(const bool dec_speed_down){
    const int send_msg = dec_speed_down ? JPEG_PARAM_UP : JPEG_PARAM_KEEP;
    for(int i=0; i<this->display_num; ++i){
        _asio::async_write(*this->socks[i],
                           _asio::buffer(std::to_string(send_msg)+MSG_DELIMITER),
                           boost::bind(&SyncManager::onSendSync, this, _ph::error, _ph::bytes_transferred, i)
        );
    }
}

/* 同期メッセージの受信を開始 */
void SyncManager::run(){
    this->pre_t = _chrono::high_resolution_clock::now();
    for(int i=0; i<this->display_num; ++i){
        _asio::async_read_until(*this->socks[i],
                                *this->stream_bufs[i],
                                MSG_DELIMITER,
                                boost::bind(&SyncManager::onRecvSync, this, _ph::error, _ph::bytes_transferred, i)
        );
    }
    this->ios.run();
}

