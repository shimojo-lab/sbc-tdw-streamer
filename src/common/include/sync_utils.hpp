/*******************************
*        sync_utils.hpp        *
*  (同期通信用ヘッダファイル)  *
*******************************/

#ifndef SYNC_UTILS_HPP
#define SYNC_UTILS_HPP

extern "C"{
    #include <turbojpeg.h>
}

const int JPEG_TUNING_ON = 1;      // JPEGパラメータ変更フラグ
const int JPEG_TUNING_OFF = 0;     // JPEGパラメータ非変更フラグ
const int JPEG_PARAM_KEEP = 0;     // JPEGパラメータ維持フラグ
const int JPEG_PARAM_UP = 1;       // JPEGパラメータ向上フラグ
const int JPEG_PARAM_DOWN = -1;    // JPEGパラメータ低下フラグ
const int JPEG_QUALITY_MAX = 100;  // JPEG品質係数の最大値
const int JPEG_QUALITY_MIN = 0;    // JPEG品質係数の最小値

#endif  /* SYNC_UTILS_HPP */

