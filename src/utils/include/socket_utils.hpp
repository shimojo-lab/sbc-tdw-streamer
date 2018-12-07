/*********************************
*        socket_utils.hpp        *
* (ソケット通信用ヘッダファイル) *
*********************************/

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

extern const std::string SEQ_DELIMITER;  // シーケンス番号部のデリミタ
extern const int SEQ_DELIMITER_LEN;      // シーケンス番号部のデリミタ長
extern const std::string MSG_DELIMITER;  // 受信メッセージのデリミタ
extern const int MSG_DELIMITER_LEN;      // 受信メッセージのデリミタ長

#endif  /* SOCKET_UTILS_HPP */

