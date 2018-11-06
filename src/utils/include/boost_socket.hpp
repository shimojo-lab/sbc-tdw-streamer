/*********************************
*       boost_socket.hpp         *
* (Boost.Asioによるソケット通信) *
*********************************/

#ifndef BOOST_SOCKET_HPP
#define BOOST_SOCKET_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/barrier.hpp>

namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _ph = boost::asio::placeholders;
using _ios = _asio::io_service;
using _tcp = _ip::tcp;
using _udp = _ip::udp;
using _err = boost::system::error_code;
using tcps_ptr_t = std::shared_ptr<_tcp::socket>;
using acc_ptr_t = std::shared_ptr<_tcp::acceptor>;
using bar_ptr_t = std::shared_ptr<boost::barrier>;

extern const std::string SEPARATOR;  // 受信メッセージのセパレータ
extern const int SEPARATOR_LEN;      // 受信メッセージのセパレータの長さ

#endif  /* BOOST_SOCKET_HPP */

