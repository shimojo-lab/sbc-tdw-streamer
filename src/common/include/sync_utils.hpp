/******************************************
*             sync_utils.hpp              *
*  (the utilities to pass sync messages)  *
******************************************/

#ifndef SYNC_UTILS_HPP
#define SYNC_UTILS_HPP

#include <vector>
#include <atomic>
#include <chrono>

namespace _chrono = std::chrono;

using jpeg_params_t = std::vector<std::atomic_int>;
using hr_clock_t = _chrono::high_resolution_clock::time_point;

const int VIEWBUF_EXTRA_NUM = 3;    // the number of extra domains in the view buffer
const int JPEG_NO_CHANGE = 0;       // the flag not to change the JPEG parameters
const int JPEG_QUALITY_CHANGE = 1;  // the flag to change the quality factor
const int JPEG_YCbCr_CHANGE = 2;    // the flag to change the YCbCr format
const int JPEG_PARAM_KEEP = 0;      // the flag to keep the JPEG parameter
const int JPEG_PARAM_UP = 1;        // the flag to increase the JPEG parameter
const int JPEG_PARAM_DOWN = 2;      // the flag to decrease the JPEG parameter
const int JPEG_QUALITY_MIN = 1;     // the minimum value of the quality factor
const int JPEG_QUALITY_MAX = 100;   // the maximum value of the quality factor

#endif  /* SYNC_UTILS_HPP */

