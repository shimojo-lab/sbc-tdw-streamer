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
void SyncManager::parseSyncMsg(const std::string& msg, const int id){
    this->json.deserialize(msg);
    const int frame_num = this->json.getParam("num");
    const int tune = this->json.getParam("tune");
    
    if(tune == JPEG_TUNING_ON){
        const int param = this->json.getParam("param");
        const int change = this->json.getParam("change");
        if(param == JPEG_QUALITY_CHANGE){
            // 品質係数を変更
            if(change==JPEG_PARAM_UP && this->quality.load()<JPEG_QUALITY_MAX){
                ++this->quality;
            }
            else if(change==JPEG_PARAM_DOWN && this->quality.load()>JPEG_QUALITY_MIN){
                --this->quality;
            }
        }else{
            // クロマサブサンプル比を変更 
            if(change==JPEG_PARAM_UP){
                switch(this->sampling_type.load()){
                    case TJSAMP_422:
                        this->sampling_type.store(TJSAMP_444);
                        break;
                    case TJSAMP_420:
                        this->sampling_type.store(TJSAMP_422);
                        break;
                }
            }else if(change==JPEG_PARAM_DOWN){
                switch(this->sampling_type.load()){
                    case TJSAMP_444:
                        this->sampling_type.store(TJSAMP_422);
                        break;
                    case TJSAMP_422:
                        this->sampling_type.store(TJSAMP_420);
                        break;
                }
            }
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
    ++this->sync_count;
    if(this->sync_count.load(std::memory_order_acquire) == this->display_num){
        this->sync_count.store(0);
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
    std::string send_msg("sync");
    send_msg += MSG_DELIMITER;
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

