/*******************************
*        sync_utils.hpp        *
*  (同期通信用ヘッダファイル)  *
*******************************/

#ifndef SYNC_UTILS_HPP
#define SYNC_UTILS_HPP

#include <vector>
#include <atomic>
#include <chrono>

namespace _chrono = std::chrono;

using jpeg_params_t = std::vector<std::atomic_int>;
using hr_clock_t = _chrono::high_resolution_clock::time_point;

const int VIEWBUF_EXTRA_NUM = 3;      // 表示フレームバッファの余剰領域数
const int JPEG_NO_CHANGE = 0;         // 変更パラメータ未指定フラグ
const int JPEG_QUALITY_CHANGE = 1;    // 品質係数変更フラグ
const int JPEG_YUV_CHANGE = 2;        // YUVサンプル比変更フラグ
const int JPEG_PARAM_KEEP = 0;        // JPEGパラメータ維持フラグ
const int JPEG_PARAM_UP = 1;          // JPEGパラメータ向上フラグ
const int JPEG_PARAM_DOWN = 2;        // JPEGパラメータ低下フラグ
const int JPEG_QUALITY_MIN = 1;       // 品質係数の最小値
const int JPEG_QUALITY_MAX = 100;     // 品質係数の最大値

#endif  /* SYNC_UTILS_HPP */

