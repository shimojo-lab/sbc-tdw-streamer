##################################
#            Makefile            #
#   (sbc_tdw_streamerビルド用)   #
##################################

CXX = g++
CXXFLAGS = -Wall -std=c++11 -O3
HEAD = $(PWD)/src/head
DISP = $(PWD)/src/display
COMN = $(PWD)/src/common
CONF = $(PWD)/conf
BIN = $(PWD)/bin
CV_HDR = /usr/local/include/opencv2
HEAD_LDFLAGS = -lboost_system -lboost_thread -lpthread -lturbojpeg \
               -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio 
DISP_LDFLAGS = -lboost_system -lboost_thread -lpthread -lturbojpeg \
               -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc

# 全てビルド
.PHONY: all
all: build_common build_head build_display

# ヘッドノードでのビルドとテスト
.PHONY: head
head: build_common build_head test_head

# ディスプレイノードでのビルドとテスト
.PHONY: display
display: build_common build_display test_display

# 共通モジュールのビルド
.PHONY: build_common
build_common: $(COMN)/mutex_logger.o $(COMN)/base_config_parser.o

$(COMN)/mutex_logger.o: $(COMN)/mutex_logger.cpp
	$(CXX) $(CXXFLAGS) -I$(COMN)/include -c -o $@ $<

$(COMN)/base_config_parser.o: $(COMN)/base_config_parser.cpp
	$(CXX) $(CXXFLAGS) -I$(COMN)/include -c -o $@ $<

# ヘッドノード用プログラムをビルド
.PHONY: build_head
build_head: $(COMN)/mutex_logger.o $(COMN)/base_config_parser.o $(HEAD)/config_parser.o \
            $(HEAD)/frame_encoder.o $(HEAD)/frame_sender.o $(HEAD)/sync_manager.o \
            $(HEAD)/frontend_server.o $(HEAD)/main.o
	$(CXX) $(HEAD_LDFLAGS) -o $(BIN)/head_server $^

$(HEAD)/config_parser.o: $(HEAD)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I$(HEAD)/include -I$(COMN)/include -c -o $@ $<

$(HEAD)/frame_encoder.o: $(HEAD)/frame_encoder.cpp
	$(CXX) $(CXXFLAGS) -I$(HEAD)/include -I$(COMN)/include -I$(CV_HDR) -c -o $@ $<

$(HEAD)/frame_sender.o: $(HEAD)/frame_sender.cpp
	$(CXX) $(CXXFLAGS) -I$(HEAD)/include -I$(COMN)/include -c -o $@ $<

$(HEAD)/sync_manager.o: $(HEAD)/sync_manager.cpp
	$(CXX) $(CXXFLAGS) -I$(HEAD)/include -I$(COMN)/include -c -o $@ $<

$(HEAD)/frontend_server.o: $(HEAD)/frontend_server.cpp
	$(CXX) $(CXXFLAGS) -I$(HEAD)/include -I$(COMN)/include -c -o $@ $<

$(HEAD)/main.o: $(HEAD)/main.cpp
	$(CXX) $(CXXFLAGS) -I$(HEAD)/include -I$(COMN)/include -c -o $@ $<

# ディスプレイノード用プログラムをビルド
.PHONY: build_display
build_display: $(COMN)/mutex_logger.o $(COMN)/base_config_parser.o $(DISP)/config_parser.o \
               $(DISP)/memory_checker.o $(DISP)/frame_receiver.o $(DISP)/frame_decoder.o \
               $(DISP)/frame_viewer.o $(DISP)/display_client.o $(DISP)/main.o
	$(CXX) $(DISP_LDFLAGS) -o $(BIN)/display_client $^

$(DISP)/config_parser.o: $(DISP)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I$(DISP)/include -I$(COMN)/include -c -o $@ $<

$(DISP)/memory_checker.o: $(DISP)/memory_checker.cpp
	$(CXX) $(CXXFLAGS) -I$(DISP)/include -c -o $@ $<

$(DISP)/frame_receiver.o: $(DISP)/frame_receiver.cpp
	$(CXX) $(CXXFLAGS) -I$(DISP)/include -I$(COMN)/include -c -o $@ $<

$(DISP)/frame_decoder.o: $(DISP)/frame_decoder.cpp
	$(CXX) $(CXXFLAGS) -I$(DISP)/include -I$(COMN)/include -I$(CV_HDR) -c -o $@ $<

$(DISP)/frame_viewer.o: $(DISP)/frame_viewer.cpp
	$(CXX) $(CXXFLAGS) -I$(DISP)/include -I$(COMN)/include -I$(CV_HDR) -c -o $@ $<

$(DISP)/display_client.o: $(DISP)/display_client.cpp
	$(CXX) $(CXXFLAGS) -I$(DISP)/include -I$(COMN)/include -c -o $@ $<

$(DISP)/main.o: $(DISP)/main.cpp
	$(CXX) $(CXXFLAGS) -I$(DISP)/include -I$(COMN)/include -c -o $@ $<

# ヘッドノード用プログラムを起動
.PHONY: test_head
test_head:
	$(BIN)/head_server $(CONF)/head_conf.json

# ディスプレイノード用プログラムを起動
.PHONY: test_display
test_display:
	$(BIN)/display_client $(CONF)/display_conf.json

# 実行ファイルを全消去
.PHONY: clean
clean:
	rm -f $(BIN)/*
	rm -f $(HEAD)/*.o
	rm -f $(DISP)/*.o
	rm -f $(COMN)/*.o

