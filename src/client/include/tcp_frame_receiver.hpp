/*******************************
 *    tcp_frame_receiver.hpp   *
 *    (TCP版フレーム受信器)    *
 ******************************/

#ifndef TCP_FRAME_RECEIVER_HPP
#define TCP_FRAME_RECEIVER_HPP

#include "base_frame_receiver.hpp"

/* TCP版フレーム受信器 */
class TCPFrameReceiver : public BaseFrameReceiver{
    private:
        const tcps_ptr_t sock;      // TCPソケット
        _asio::streambuf recv_buf;  // TCP受信用バッファ
        
        void startConnect(const std::string ip, const int port) override;  // TCP接続待機を開始
        void onConnect(const _err& err) override;                          // TCP接続時のコールバック
        void onRecvFrame(const _err& err, size_t t_bytes) override;        // フレーム受信時のコールバック
    
    public:
        TCPFrameReceiver(_ios& ios, _ios::strand& strand, const tcps_ptr_t sock, const fq_ptr_t queue, const std::string ip, const int port);  // コンストラクタ
};

#endif  /* TCP_FRAME_RECEIVER_HPP */

