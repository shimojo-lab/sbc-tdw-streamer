/******************************
 *   udp_frame_receiver.hpp   *
 *   (UDP版フレーム受信器)    *
 *****************************/

#ifndef UDP_FRAME_RECEIVER_HPP
#define UDP_FRAME_RECEIVER_HPP

#include "base_frame_receiver.hpp"

/* UDP版フレーム受信器 */
class UDPFrameReceiver : public BaseFrameReceiver{
    private:
        tcp_t::socket sock;         // TCPソケット
        _asio::streambuf recv_buf;  // TCP受信用バッファ
        
        void startConnect(const std::string ip, const int port) override;  // TCP接続待機を開始
        void onConnect(const err_t& err) override;                          // TCP接続時のコールバック
        void onRecvFrame(const err_t& err, size_t t_bytes) override;        // フレーム受信時のコールバック
    
    public:
        UDPFrameReceiver(ios_t& ios, const fq_ptr_t queue, const std::string ip, const int port);  // コンストラクタ
};

#endif  /* UDP_FRAME_RECEIVER_HPP */

