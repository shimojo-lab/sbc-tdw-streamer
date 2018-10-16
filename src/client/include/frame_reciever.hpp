/** frame_reciever.hpp **/

#include <stdlib.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace asio = boost::asio;

/* フレーム受信器 */
class FrameReciever: public boost::enable_shared_from_this<FrameReciever>{
	private:
        asio::io_service *ios;         // BoostのI/Oオブジェクト
		asio::ip::tcp::socket *sock;   // 受信用のTCPソケット
        asio::ip::tcp::acceptor *acc;  // BoostのTCP受信器
        asio::streambuf recieve_buf;   // 受信用バッファ
	public:
		FrameReciever(asio::io_service *ios, int port);  // コンストラクタ
		~FrameReciever();  // デストラクタ
		void waitForConnection();  // 受信待機を開始
		void callbackOnConnection(const boost::system::error_code &error);  // コールバック
		void callbackOnRecieve(const boost::system::error_code &error, size_t recieve_size);  // コールバック
};

