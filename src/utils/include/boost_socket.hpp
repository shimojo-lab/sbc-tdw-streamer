/*********************************
*       boost_socket.hpp         *
* (Boost.Asioによるソケット通信) *
*********************************/

#ifndef BOOST_SOCKET_HPP
#define BOOST_SOCKET_HPP

#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace _asio = boost::asio;
namespace _ip = boost::asio::ip;
namespace _ph = boost::asio::placeholders;
using ios_t = _asio::io_service;
using tcp_t = _ip::tcp;
using tcps_ptr_t = std::shared_ptr<tcp_t::socket>;
using acc_ptr_t = std::shared_ptr<tcp_t::acceptor>;
using udp_t = _ip::udp;
using err_t = boost::system::error_code;

extern const std::string SEPARATOR;  // 受信メッセージのセパレータ
extern const int SEPARATOR_LEN;      // 受信メッセージのセパレータの長さ

#endif  /* BOOST_SOCKET_HPP */

