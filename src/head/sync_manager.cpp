/**************************************
*          sync_manager.cpp           *
*  (synchronization process manager)  *
**************************************/

#include "sync_manager.hpp"

/* constructor */
SyncManager::SyncManager(_asio::io_service& ios, std::vector<sock_ptr_t>& socks,
                         jpeg_params_t& ycbcr_format_list, jpeg_params_t& quality_list):
    ios(ios),
    socks(socks),
    display_num(socks.size()),
    sync_count(0),
    ycbcr_format_list(ycbcr_format_list),
    quality_list(quality_list)
{
    for(int i=0; i<this->display_num; ++i){
        this->stream_bufs.push_back(std::make_shared<_asio::streambuf>());
    }
}

/* change the YCbCr format */
const std::string SyncManager::changeYCbCr(const int change_flag, const int id){
    std::string new_type = "";
    switch(change_flag){
    case JPEG_PARAM_UP:
        switch(this->ycbcr_format_list[id].load(std::memory_order_acquire)){
        case TJSAMP_422:
            this->ycbcr_format_list[id].store(TJSAMP_444, std::memory_order_release);
            new_type = "4:4:4";
            break;
        case TJSAMP_420:
            this->ycbcr_format_list[id].store(TJSAMP_422, std::memory_order_release);
            new_type = "4:2:2";
            break;
        }
        break;
    case JPEG_PARAM_DOWN:
        switch(this->ycbcr_format_list[id].load(std::memory_order_acquire)){
        case TJSAMP_444:
            this->ycbcr_format_list[id].store(TJSAMP_422, std::memory_order_release);
            new_type = "4:2:2";
            break;
        case TJSAMP_422:
            this->ycbcr_format_list[id].store(TJSAMP_420, std::memory_order_release);
            new_type = "4:2:0";
            break;
        }
        break;
    }
    return new_type;
}

/* change the quality factor */
const std::string SyncManager::changeQuality(const int change_flag, const int id){
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

/* parse a sync message */
void SyncManager::parseSyncMsg(const std::string& sync_msg, const int id){
    this->sync_params.deserialize(sync_msg);
    const int param_flag = this->sync_params.getIntParam("param");
    const int change_flag = this->sync_params.getIntParam("change");
    
    if(param_flag == JPEG_YCbCr_CHANGE){
        const std::string new_type = this->changeYCbCr(change_flag, id);
        if(new_type != ""){
            _ml::notice("Display"+std::to_string(id)+": "+"YCbCr format is changed to "+new_type);
        }
    }else if(param_flag == JPEG_QUALITY_CHANGE){
        std::string new_quality = this->changeQuality(change_flag, id);
        if(new_quality != ""){
            _ml::notice("Display"+std::to_string(id)+": "+"Quality is changed to "+new_quality);
        }
    }
}

/* the callback when receiving a sync message */
void SyncManager::onRecvSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        _ml::caution("Failed to receive sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // parse a sync message
    const auto data = this->stream_bufs[id]->data();
    std::string sync_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    sync_msg.erase(sync_msg.length()-MSG_DELIMITER_LEN);
    this->parseSyncMsg(sync_msg, id);
    this->stream_bufs[id]->consume(t_bytes);
    
    // synchronize all the display nodes
    this->sync_count.fetch_add(1, std::memory_order_release);
    if(this->sync_count.load(std::memory_order_acquire) == this->display_num){
        // calculate the current frame rate
        ++this->frame_count;
        if(this->frame_count%FPS_INTERVAL == 0){
            const hr_clock_t post_t = _chrono::high_resolution_clock::now();
            const double fps = 1000.0 * (double)FPS_INTERVAL / _chrono::duration_cast<_chrono::milliseconds>(post_t-this->pre_t).count();
            _ml::notice(std::to_string(this->frame_count) + ": " + std::to_string(fps) + "fps");
            this->pre_t = post_t;
        }
        this->sync_count.store(0, std::memory_order_release);
        this->sendSync();
    }
}

/* the callback when sending a sync message */
void SyncManager::onSendSync(const err_t& err, size_t t_bytes, const int id){
    if(err){
        _ml::caution("Failed to send sync message", err.message());
        std::exit(EXIT_FAILURE);
    }
    
    // restart receiving sync messages
    _asio::async_read_until(*this->socks[id],
                            *this->stream_bufs[id],
                            MSG_DELIMITER,
                            boost::bind(&SyncManager::onRecvSync, this, _ph::error, _ph::bytes_transferred, id)
    );
}

/* send a sync message */
void SyncManager::sendSync(){
    const std::string send_msg = "sync" + MSG_DELIMITER;
    for(int i=0; i<this->display_num; ++i){
        _asio::async_write(*this->socks[i],
                           _asio::buffer(send_msg),
                           boost::bind(&SyncManager::onSendSync, this, _ph::error, _ph::bytes_transferred, i)
        );
    }
}

/* start the synchronization process */
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

