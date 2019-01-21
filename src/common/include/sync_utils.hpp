/*******************************
*        sync_utils.hpp        *
*  (同期通信用ヘッダファイル)  *
*******************************/

#ifndef SYNC_UTILS_HPP
#define SYNC_UTILS_HPP

#include <chrono>

using hr_chrono_t = std::chrono::high_resolution_clock::time_point;

const int VIEWBUF_EXTRA_NUM = 3;     // 表示フレームバッファの余剰領域数
const int JPEG_TUNING_OFF = 0;       // JPEGパラメータ非変更フラグ
const int JPEG_TUNING_ON = 1;        // JPEGパラメータ変更フラグ
const int JPEG_NO_CHANGE = 0;        // 変更パラメータ未指定フラグ
const int JPEG_QUALITY_CHANGE = 1;   // 品質係数変更フラグ
const int JPEG_SAMPLING_CHANGE = 2;  // クロマサブサンプル比変更フラグ
const int JPEG_PARAM_KEEP = 0;       // JPEGパラメータ維持フラグ
const int JPEG_PARAM_UP = 1;         // JPEGパラメータ向上フラグ
const int JPEG_PARAM_DOWN = 2;       // JPEGパラメータ低下フラグ
const int JPEG_QUALITY_MIN = 1;      // 品質係数の最小値
const int JPEG_QUALITY_MAX = 100;    // 品質係数の最大値

#endif  /* SYNC_UTILS_HPP */

