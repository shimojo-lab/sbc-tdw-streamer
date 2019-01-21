/*************************
*    sync_manager.cpp    *
*      (同期制御器)      *
*************************/

#include "sync_manager.hpp"

/* コンストラクタ */
SyncManager::SyncManager(_asio::io_service& ios, std::vector<sock_ptr_t>& socks,
                         std::atomic<int>& sampling_type, std::atomic<int>& quality):
    ios(ios),
    socks(socks),
    display_num(socks.size()),
    sampling_type(sampling_type),
    quality(quality)
{
    // パラメータを初期化
    this->sync_count.store(0, std::memory_order_release);
    for(int i=0; i<this->display_num; ++i){
        this->stream_bufs.push_back(std::make_shared<_asio::streambuf>());
    }
}

/* 同期メッセージをパース */
void SyncManager::parseSyncMsg(const std::string& sync_msg, const int id){
    this->sync_params.deserialize(sync_msg);
    const int param_flag = this->sync_params.getIntParam("param");
    const int change_flag = this->sync_params.getIntParam("change");
    
    if(param_flag == JPEG_SAMPLING_CHANGE){
        // クロマサブサンプル比を変更 
        if(change_flag == JPEG_PARAM_UP){
            switch(this->sampling_type.load(std::memory_order_acquire)){
                case TJSAMP_422:
                    this->sampling_type.store(TJSAMP_444, std::memory_order_release);
                    break;
                case TJSAMP_420:
                    this->sampling_type.store(TJSAMP_422, std::memory_order_release);
                    break;
            }
        }else if(change_flag == JPEG_PARAM_DOWN){
            switch(this->sampling_type.load(std::memory_order_acquire)){
                case TJSAMP_444:
                    this->sampling_type.store(TJSAMP_422);
                    break;
                case TJSAMP_422:
                    this->sampling_type.store(TJSAMP_420);
                    break;
            }
        }
    }else if(param_flag == JPEG_QUALITY_CHANGE){
        // 品質係数を変更
        if(change_flag==JPEG_PARAM_UP && this->quality.load(std::memory_order_acquire)<JPEG_QUALITY_MAX){
            this->quality.fetch_add(1, std::memory_order_release);
        }else if(change_flag==JPEG_PARAM_DOWN && this->quality.load(std::memory_order_acquire)>JPEG_QUALITY_MIN){
            this->quality.fetch_sub(1, std::memory_order_release);
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
    std::string sync_msg(_asio::buffers_begin(data), _asio::buffers_begin(data)+t_bytes);
    sync_msg.erase(sync_msg.length()-MSG_DELIMITER_LEN);
    this->parseSyncMsg(sync_msg, id);
    this->stream_bufs[id]->consume(t_bytes);
    
    // 全ディスプレイノード間で同期
    this->sync_count.fetch_add(1, std::memory_order_release);
    if(this->sync_count.load(std::memory_order_acquire) == this->display_num){
        const auto post_time = std::chrono::system_clock::now();
        double elapsed = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(post_time-this->pre_time).count();
        _ml::debug(elapsed);
        this->pre_time = post_time;
        this->sync_count.store(0, std::memory_order_release);
        this->sendSync();
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
void SyncManager::sendSync(){
    const std::string send_msg = std::to_string(this->next_id) + MSG_DELIMITER;
    for(int i=0; i<this->display_num; ++i){
        _asio::async_write(*this->socks[i],
                           _asio::buffer(send_msg),
                           boost::bind(&SyncManager::onSendSync, this, _ph::error, _ph::bytes_transferred, i)
        );
    }
}

/* 同期メッセージの受信を開始 */
void SyncManager::run(){
    for(int i=0; i<this->display_num; ++i){
        _asio::async_read_until(*this->socks[i],
                                *this->stream_bufs[i],
                                MSG_DELIMITER,
                                boost::bind(&SyncManager::onRecvSync, this, _ph::error, _ph::bytes_transferred, i)
        );
    }
    this->ios.run();
}

