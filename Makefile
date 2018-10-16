# Makefile

CXX = g++
OBJS = pi_tdw_streamer
SERVER_SRC = ./src/server
CLIENT_SRC = ./src/client
BIN = ./bin
CV2_HEADER = /usr/local/include/opencv2
CXXFLAGS = -Wall -std=c++11
SERVER_LD = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_videoio -l boost_system -l pthread
CLIENT_LD = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l SDL2 -l boost_system -l pthread

# コンパイルとテストを一括実行
.PHONY: all
all: server client

# サーバのコンパイルとテスト
.PHONY: server
server: compile_server test_server

# クライアントのコンパイルとテスト
.PHONY: client
client: compile_client test_client

# サーバをコンパイル
.PHONY: compile_server
compile_server: $(SERVER_SRC)/main.o $(SERVER_SRC)/video_demuxer.o $(SERVER_SRC)/frame_streamer.o 
	$(CXX) $(CXXFLAGS) $(SERVER_LD) -o $(BIN)/server $^

$(SERVER_SRC)/main.o: $(SERVER_SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(SERVER_SRC)/include -c -o $@ $<

$(SERVER_SRC)/video_demuxer.o: $(SERVER_SRC)/video_demuxer.cpp
	$(CXX) $(CXXFLAGS) -I $(SERVER_SRC)/include -I $(CV2_HEADER) -c -o $@ $<

$(SERVER_SRC)/frame_streamer.o: $(SERVER_SRC)/frame_streamer.cpp
	$(CXX) $(CXXFLAGS) -I $(SERVER_SRC)/include -I $(CV2_HEADER) -c -o $@ $<

# クライアントをコンパイル
.PHONY: compile_client
compile_client: $(CLIENT_SRC)/main.o $(CLIENT_SRC)/frame_reciever.o $(CLIENT_SRC)/frame_viewer.o
	$(CXX) $(CXXFLAGS) $(CLIENT_LD) -o $(BIN)/client $^

$(CLIENT_SRC)/main.o: $(CLIENT_SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(CLIENT_SRC)/include -c -o $@ $<

$(CLIENT_SRC)/frame_reciever.o: $(CLIENT_SRC)/frame_reciever.cpp
	$(CXX) $(CXXFLAGS) -I $(CLIENT_SRC)/include -c -o $@ $<

$(CLIENT_SRC)/frame_viewer.o: $(CLIENT_SRC)/frame_viewer.cpp
	$(CXX) $(CXXFLAGS) -I $(CLIENT_SRC)/include -I $(CV2_HEADER) -c -o $@ $<

# サーバを起動
.PHONY: test_server
test_server:	
	$(BIN)/server

# クライアントを起動
.PHONY: test_client
test_client:
	$(BIN)/client

# 実行ファイルを全消去
.PHONY: clean
clean:
	rm -f $(BIN)/*
	rm -f $(SERVER_SRC)/*.o
	rm -f $(CLIENT_SRC)/*.o

