/** frame_reciever.cpp **/

#include "frame_reciever.hpp"

// コンストラクタ
FrameReciever::FrameReciever(asio::io_service *ios, int port){
    // 受信用のTCPソケットを用意
    asio::ip::tcp::socket sock(*ios);
    asio::ip::tcp::acceptor acc(*ios, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    this->sock = &sock;
    this->acc = &acc;
    //this->waitForConnection();
}

// デストラクタ
FrameReciever::~FrameReciever(){
	
}

// 接続待機を開始するメソッド
void FrameReciever::waitForConnection(){
    this->acc->async_accept(
        *(this->sock),
        boost::bind(&FrameReciever::callbackOnConnection, shared_from_this(), asio::placeholders::error)
    );
    return;
}

// 接続時のコールバック
void FrameReciever::callbackOnConnection(const boost::system::error_code &error){
    // 接続失敗時のエラー処理
    if(error){
        std::cout << "[Error] Connection failed. (" << error.message() << ")" << std::endl;
        exit(-1);
    }
    
    // 受信処理をループ
    while(true){
        asio::async_read_until(
            *(this->sock),
            this->recieve_buf,
            "\r\n\r\n",
            boost::bind(&FrameReciever::callbackOnRecieve, shared_from_this(), asio::placeholders::error, asio::placeholders::bytes_transferred)
        );
    }
    return;
}

// 受信時のコールバック
void FrameReciever::callbackOnRecieve(const boost::system::error_code &error, size_t recieve_size){
    if(error){
        std::cerr << "[Error] Receive failed. (" << error.message() << ")" << std::endl;
    }else{
        const unsigned char *frame = asio::buffer_cast<const unsigned char*>(this->recieve_buf.data());
        std::cout << *frame << std::endl;
        //this->recieve_buf.consume(len);
    }
    return;
}

