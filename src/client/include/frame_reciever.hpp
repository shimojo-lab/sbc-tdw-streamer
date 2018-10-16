/** frame_reciever.hpp **/

#include <boost/asio.hpp>

namespace asio = boost::asio;

/* フレーム受信器 */
class FrameReciever{
	private:
		asio::ip::tcp::socket *sock;  // TCPのソケット
		boost::system::error_code error;  // Boostのエラーコード
	public:
		FrameReciever();  // コンストラクタ
		~FrameFreciever();  // デストラクタ
		initTCPSocket();  // 受信用のTCPソケットを用意
		
};

