/***********************************
*         socket_utils.hpp         *
*  (ソケット通信用ヘッダファイル)  *
***********************************/

#ifndef SOCKET_UTILS_HPP
#define SOCKET_UTILS_HPP

#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _ph = boost::asio::placeholders;

using sock_ptr_t = std::shared_ptr<_ip::tcp::socket>;
using acc_ptr_t = std::shared_ptr<_ip::tcp::acceptor>;
using err_t = boost::system::error_code;

const std::string MSG_DELIMITER = "--EOM\r\n";         // 送受信メッセージのデリミタ
const int MSG_DELIMITER_LEN = MSG_DELIMITER.length();  // 送受信メッセージのデリミタ長

#endif  /* SOCKET_UTILS_HPP */

