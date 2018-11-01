##################################
#            Makefile            #
#   (sbc_tdw_streamerビルド用)   #
##################################

CXX = g++
CXXFLAGS = -Wall -std=c++11
SRV_SRC = ./src/server
CLI_SRC = ./src/client
UTILS_SRC = ./src/utils
BIN = ./bin
CONF = ./conf
CV2_HDR = /usr/local/include/opencv2
SRV_LD = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_videoio -l boost_system -l boost_thread -l pthread
CLI_LD = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_imgproc -l SDL2 -l boost_system -l boost_thread -l pthread

# 全てのコンパイルとテストを一括実行
.PHONY: all
all: server client

# 送信サーバのコンパイルとテスト
.PHONY: server
server: compile_server test_server

# 表示クライアントのコンパイルとテスト
.PHONY: client
client: compile_client test_client

# 送信サーバをコンパイル
.PHONY: compile_server
compile_server: $(SRV_SRC)/main.o $(UTILS_SRC)/print_with_mutex.o $(SRV_SRC)/config_parser.o $(SRV_SRC)/frame_queue.o $(SRV_SRC)/video_splitter.o $(SRV_SRC)/frontend_server.o $(SRV_SRC)/frame_sender.o 
	$(CXX) $(CXXFLAGS) $(SRV_LD) -o $(BIN)/sbc_server $^

$(SRV_SRC)/main.o: $(SRV_SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -I $(UTILS_SRC)/include -c -o $@ $<

$(UTILS_SRC)/print_with_mutex.o: $(UTILS_SRC)/print_with_mutex.cpp
	$(CXX) $(CXXFLAGS) -I $(UTILS_SRC)/include -c -o $@ $<

$(SRV_SRC)/config_parser.o: $(SRV_SRC)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -I $(UTILS_SRC)/include -c -o $@ $<

$(SRV_SRC)/frame_queue.o: $(SRV_SRC)/frame_queue.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -c -o $@ $<

$(SRV_SRC)/video_splitter.o: $(SRV_SRC)/video_splitter.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -I $(UTILS_SRC)/include -I $(CV2_HDR) -c -o $@ $<

$(SRV_SRC)/frontend_server.o: $(SRV_SRC)/frontend_server.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -c -I $(UTILS_SRC)/include -o $@ $<

$(SRV_SRC)/frame_sender.o: $(SRV_SRC)/frame_sender.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -I $(UTILS_SRC)/include -I $(CV2_HDR) -c -o $@ $<

# 表示クライアントをコンパイル
.PHONY: compile_client
compile_client: $(CLI_SRC)/main.o $(CLI_SRC)/config_parser.o $(CLI_SRC)/frame_queue.o $(CLI_SRC)/frame_receiver.o $(CLI_SRC)/frame_viewer.o
	$(CXX) $(CXXFLAGS) $(CLI_LD) -o $(BIN)/sbc_client $^

$(CLI_SRC)/main.o: $(CLI_SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -c -o $@ $<

$(CLI_SRC)/config_parser.o: $(CLI_SRC)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -c -o $@ $<

$(CLI_SRC)/frame_queue.o: $(CLI_SRC)/frame_queue.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -c -o $@ $<

$(CLI_SRC)/frame_receiver.o: $(CLI_SRC)/frame_receiver.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -c -o $@ $<

$(CLI_SRC)/frame_viewer.o: $(CLI_SRC)/frame_viewer.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -I $(CV2_HDR) -c -o $@ $<

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
	rm -f $(SRV_SRC)/*.o
	rm -f $(CLI_SRC)/*.o

