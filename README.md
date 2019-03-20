# SBC-TDW-Streamer
A middleware to build a Tiled Display Wall (TDW) with Single Board Computers (SBC)

## Setup
## Head node (a Linux server)
1. Install the dependencies.
  - g++ compiler
  - make
  - [TurboJPEG](https://cdn.rawgit.com/libjpeg-turbo/libjpeg-turbo/master/doc/html/group___turbo_j_p_e_g.html) (select C/C++ API)
  - [FFmpeg](https://github.com/FFmpeg/FFmpeg)
  - [OpenCV C++ libraries](https://github.com/opencv/opencv) (with FFmpeg support)
  - [Boost C++ libraries](https://www.boost.org)
2. Clone this repository, and move into the directory.
3. Run `make head`.

## Display node (Raspberry Pi, TinkerBoard, ... etc.)
1. Install the dependencies.
  - g++ compiler
  - make
  - [TurboJPEG](https://cdn.rawgit.com/libjpeg-turbo/libjpeg-turbo/master/doc/html/group___turbo_j_p_e_g.html) (select C/C++ API)
  - [Boost C++ Libraries](https://www.boost.org)
  - [FBDEV](https://www.x.org/archive/X11R6.8.0/doc/fbdev.4.html)
2. Clone this repository, and move into the directory.
3. Run `make display`.

## How to run
1. On the head node,
  - Edit `conf/head_conf.json`.
  - Run `bin/head_server conf/head_conf.json`. (`make test_head` is also available)
2. On each display node,
  - Edit `conf/display_conf.json`.
  - Run `bin/display_client conf/display_conf.json`. (`make test_display` is also available)
3. When all the display nodes finish connecting to the head node, the video playback is started on the TDW.

