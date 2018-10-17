# Makefile 
# (pi_tdw_streamerビルド用)

CXX = g++
CXXFLAGS = -Wall -std=c++11
SRV_SRC = ./src/server
CLI_SRC = ./src/client
BIN = ./bin
CONF = ./conf
CV2_HDR = /usr/local/include/opencv2
SRV_LD = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_videoio -l boost_system -l pthread
CLI_LD = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l SDL2 -l boost_system -l pthread

# コンパイルとテストを一括実行
.PHONY: all
all: server client

# 送信側のコンパイルとテスト
.PHONY: server
server: compile_server test_server

# 受信側のコンパイルとテスト
.PHONY: client
client: compile_client test_client

# サーバをコンパイル
.PHONY: compile_server
compile_server: $(SRV_SRC)/main.o $(SRV_SRC)/config_parser.o $(SRV_SRC)/video_demuxer.o $(SRV_SRC)/frame_streamer.o 
	$(CXX) $(CXXFLAGS) $(SRV_LD) -o $(BIN)/server $^

$(SRV_SRC)/main.o: $(SRV_SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -c -o $@ $<

$(SRV_SRC)/config_parser.o: $(SRV_SRC)/config_parser.cpp	
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include  -c -o $@ $<

$(SRV_SRC)/video_demuxer.o: $(SRV_SRC)/video_demuxer.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -I $(CV2_HDR) -c -o $@ $<

$(SRV_SRC)/frame_streamer.o: $(SRV_SRC)/frame_streamer.cpp
	$(CXX) $(CXXFLAGS) -I $(SRV_SRC)/include -I $(CV2_HDR) -c -o $@ $<

# クライアントをコンパイル
.PHONY: compile_client
compile_client: $(CLI_SRC)/main.o $(CLI_SRC)/config_parser.o $(CLI_SRC)/frame_receiver.o $(CLI_SRC)/frame_viewer.o
	$(CXX) $(CXXFLAGS) $(CLI_LD) -o $(BIN)/client $^

$(CLI_SRC)/main.o: $(CLI_SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -c -o $@ $<

$(CLI_SRC)/config_parser.o: $(CLI_SRC)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -c -o $@ $<

$(CLI_SRC)/frame_receiver.o: $(CLI_SRC)/frame_receiver.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -c -o $@ $<

$(CLI_SRC)/frame_viewer.o: $(CLI_SRC)/frame_viewer.cpp
	$(CXX) $(CXXFLAGS) -I $(CLI_SRC)/include -I $(CV2_HDR) -c -o $@ $<

# サーバを起動
.PHONY: test_server
test_server:
	$(BIN)/server $(CONF)/server_config.json

# クライアントを起動
.PHONY: test_client
test_client:
	$(BIN)/client $(CONF)/client_config.json

# 実行ファイルを全消去
.PHONY: clean
clean:
	rm -f $(BIN)/*
	rm -f $(SRV_SRC)/*.o
	rm -f $(CLI_SRC)/*.o

