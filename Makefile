##################################
#            Makefile            #
#   (sbc_tdw_streamerビルド用)   #
##################################

CXX = g++
CXXFLAGS = -Wall -std=c++11
SRV = ./src/server
CLI = ./src/client
UTILS = ./src/utils
BIN = ./bin
CONF = ./conf
CV2_HDR = /usr/local/include/opencv2
SRV_LD = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_videoio -l boost_system -l boost_thread
CLI_LD = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_imgproc -l SDL2 -l boost_system -l boost_thread

# 全てのコンパイルとテストを一括実行
.PHONY: all
all: server client

# 送信サーバのコンパイルとテスト
.PHONY: server
server: complie_utils compile_server test_server

# 表示クライアントのコンパイルとテスト
.PHONY: client
client: compile_utils compile_client test_client

# 共通モジュールをコンパイル
.PHONY: complie_utils
compile_utils: $(UTILS)/print_with_mutex.o $(UTILS)/sdl2_wrapper.o

$(UTILS)/print_with_mutex.o: $(UTILS)/print_with_mutex.cpp
	$(CXX) $(CXXFLAGS) -I $(UTILS)/include -c -o $@ $<

$(UTILS)/sdl2_wrapper.o: $(UTILS)/sdl2_wrapper.cpp
	$(CXX) $(CXXFLAGS) -I $(UTILS)/include -c -o $@ $<

# 送信サーバをコンパイル
.PHONY: compile_server
compile_server: $(SRV)/main.o $(UTILS)/print_with_mutex.o $(SRV)/config_parser.o $(SRV)/frame_queue.o $(SRV)/frontend_server.o $(SRV)/video_splitter.o $(SRV)/frame_sender.o 
	$(CXX) $(CXXFLAGS) $(SRV_LD) -o $(BIN)/sbc_server $^

$(SRV)/main.o: $(SRV)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV)/include -I $(UTILS)/include -c -o $@ $<

$(SRV)/config_parser.o: $(SRV)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV)/include -I $(UTILS)/include -c -o $@ $<

$(SRV)/frame_queue.o: $(SRV)/frame_queue.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV)/include -c -o $@ $<

$(SRV)/frontend_server.o: $(SRV)/frontend_server.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV)/include -c -I $(UTILS)/include -o $@ $<

$(SRV)/video_splitter.o: $(SRV)/video_splitter.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV)/include -I $(UTILS)/include -I $(CV2_HDR) -c -o $@ $<

$(SRV)/frame_sender.o: $(SRV)/frame_sender.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV)/include -I $(UTILS)/include -I $(CV2_HDR) -c -o $@ $<

# 表示クライアントをコンパイル
.PHONY: compile_client
compile_client: $(CLI)/main.o $(UTILS)/print_with_mutex.o $(UTILS)/sdl2_wrapper.o $(CLI)/config_parser.o $(CLI)/frame_queue.o $(CLI)/request_client.o $(CLI)/frame_receiver.o $(CLI)/frame_viewer.o
	$(CXX) $(CXXFLAGS) $(CLI_LD) -o $(BIN)/sbc_client $^

$(CLI)/main.o: $(CLI)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI)/include -I $(UTILS)/include -c -o $@ $<

$(CLI)/config_parser.o: $(CLI)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI)/include -I $(UTILS)/include -c -o $@ $<

$(CLI)/frame_queue.o: $(CLI)/frame_queue.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI)/include -c -o $@ $<

$(CLI)/request_client.o: $(CLI)/request_client.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI)/include -I $(UTILS)/include -c -o $@ $<

$(CLI)/frame_receiver.o: $(CLI)/frame_receiver.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI)/include -I $(UTILS)/include -c -o $@ $<

$(CLI)/frame_viewer.o: $(CLI)/frame_viewer.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI)/include -I $(UTILS)/include -I $(CV2_HDR) -c -o $@ $<

# 送信サーバを起動
.PHONY: test_server
test_server:
	$(BIN)/sbc_server $(CONF)/server_config.json

# 表示クライアントを起動
.PHONY: test_client
test_client:
	$(BIN)/sbc_client $(CONF)/client_config.json

# 実行ファイルをインストール
.PHONY: install
install: install_server install_client

install_server:
	sleep 1

install_client:
	sleep 1

# 実行ファイルを全消去
.PHONY: clean
clean:
	rm -f $(BIN)/*
	rm -f $(SRV)/*.o
	rm -f $(CLI)/*.o
	rm -f $(UTILS)/*.o

