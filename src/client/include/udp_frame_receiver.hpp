/******************************
 *   udp_frame_receiver.hpp   *
 *   (UDP版フレーム受信器)    *
 *****************************/

#ifndef UDP_FRAME_RECEIVER_HPP
#define UDP_FRAME_RECEIVER_HPP

#include "base_frame_receiver.hpp"
#include <boost/array.hpp>

/* UDP版フレーム受信器 */
class UDPFrameReceiver : public BaseFrameReceiver{
    private:
        udp_t::socket sock;                 // UDPソケット
        udp_t::endpoint endpoint;           // UDPエンドポイント
        boost::array<char, 2048> recv_buf;  // 受信用バッファ
        
        void run(const std::string ip, const int port) override;      // 受信処理を開始
        void onRecvFrame(const err_t& err, size_t t_bytes) override;  // フレーム受信時のコールバック
    
    public:
        UDPFrameReceiver(ios_t& ios, const msgbuf_ptr_t rbuf, const int port);  // コンストラクタ
};

#endif  /* UDP_FRAME_RECEIVER_HPP */

