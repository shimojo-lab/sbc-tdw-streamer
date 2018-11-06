/*********************************
*       boost_socket.hpp         *
* (Boost.Asioによるソケット通信) *
*********************************/

#ifndef BOOST_SOCKET_HPP
#define BOOST_SOCKET_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _sys = boost::system;
namespace _ph = boost::asio::placeholders;
using ios_ptr_t = std::shared_ptr<_asio::io_service>;
using tcps_ptr_t = std::shared_ptr<_ip::tcp::socket>;
using acc_ptr_t = std::shared_ptr<_ip::tcp::acceptor>;

extern const std::string SEPARATOR;  // 受信メッセージのセパレータ
extern const int SEPARATOR_LEN;      // 受信メッセージのセパレータの長さ

#endif  /* BOOST_SOCKET_HPP */

