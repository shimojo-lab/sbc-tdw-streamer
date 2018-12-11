/***********************************
*         socket_utils.hpp         *
*  (ソケット通信用ヘッダファイル)  *
***********************************/

#ifndef SOCKET_UTILS_HPP
#define SOCKET_UTILS_HPP

#include <memory>
#include <cstdlib>
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

const int TCP_STREAMING_FLAG = 0;                      // TCPフレーム送受信のフラグ
const int UDP_STREAMING_FLAG = 1;                      // UDPフレーム送受信のフラグ
const std::string MSG_DELIMITER = "--boundary\r\n";    // 送受信メッセージのデリミタ
const int MSG_DELIMITER_LEN = MSG_DELIMITER.length();  // 送受信メッセージのデリミタ長

#endif  /* SOCKET_UTILS_HPP */

