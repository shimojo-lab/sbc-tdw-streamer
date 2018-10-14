# Makefile

CXX = g++
OBJS = pi_tdw_streamer
SERVER_SRC = ./src/server
CLIENT_SRC = ./src/client
BIN = ./bin
CV2_HEADER = /usr/local/include/opencv2
CXXFLAGS = -Wall -std=c++11
CXXLINKS = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_videoio \
           -l boost_system -l pthread

# 全てコンパイル
.PHONY: all
all: server client

# サーバをコンパイル
.PHONY: server
server: $(SERVER_SRC)/main.o $(SERVER_SRC)/video_demuxer.o $(SERVER_SRC)/frame_streamer.o 
	$(CXX) $(CXXFLAGS) $(CXXLINKS) -o $(BIN)/server $^

$(SERVER_SRC)/main.o: $(SERVER_SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(SERVER_SRC)/include -c -o $@ $<

$(SERVER_SRC)/video_demuxer.o: $(SERVER_SRC)/video_demuxer.cpp
	$(CXX) $(CXXFLAGS) -I $(SERVER_SRC)/include -I $(CV2_HEADER) -c -o $@ $<

$(SERVER_SRC)/frame_streamer.o: $(SERVER_SRC)/frame_streamer.cpp
	$(CXX) $(CXXFLAGS) -I $(SERVER_SRC)/include -I $(CV2_HEADER) -c -o $@ $<

# クライアントをコンパイル
.PHONY: client
client:
	ls

# バイナリを実行
.PHONY: test
test:
	$(BIN)/server

# バイナリを全消去
.PHONY: clean
clean:
	rm -f $(BIN)/*
	rm -f $(SERVER_SRC)/*.o
	rm -f $(CLIENT_SRC)/*.o

