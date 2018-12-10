/*********************************
*        sync_utils.hpp          *
*   (同期制御用ヘッダファイル)   *
*********************************/

#ifndef SYNC_UTILS_HPP
#define SYNC_UTILS_HPP

const std::string SEC_DELIMITER = "--section\r\n";     // メッセージ領域のデリミタ
const int SEC_DELIMITER_LEN = SEC_DELIMITER.length();  // メッセージ領域のデリミタ長
const int CLI_MEM_SAFE = 1;                            // メモリ残量ありフラグ
const int CLI_MEM_DANGER = 0;                          // メモリ残量なしフラグ
const int SEND_WAIT_TIME = 1;                          // 送信中断時間

#endif  /* SYNC_UTILS_HPP */

