##################################
#            Makefile            #
#   (sbc_tdw_streamerビルド用)   #
##################################

CXX = g++
CXXFLAGS = -Wall -std=c++11 -O3
SRV = ./src/server
CLI = ./src/client
UTILS = ./src/utils
BIN = ./bin
CONF = ./conf
CV2_HDR = /usr/local/include/opencv2
SRV_LD = -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -lboost_system -lboost_thread
CLI_LD = -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lboost_system -lboost_thread -lpthread

# 全てコンパイル
.PHONY: all
all: compile_server compile_client

# ヘッドノードでのコンパイルとテスト
.PHONY: server
server: compile_server test_server

# ディスプレイノードでのコンパイルとテスト
.PHONY: client
client: compile_client test_client

# utilsのソースのコンパイル
.PHONY: compile_utils
compile_utils: $(UTILS)/print_with_mutex.o $(UTILS)/base_config_parser.o

$(UTILS)/print_with_mutex.o: $(UTILS)/print_with_mutex.cpp
	$(CXX) $(CXXFLAGS) -I$(UTILS)/include -c -o $@ $<

$(UTILS)/base_config_parser.o: $(UTILS)/base_config_parser.cpp
	$(CXX) $(CXXFLAGS) -I$(UTILS)/include -c -o $@ $<

# ヘッドノード側プログラムをコンパイル
.PHONY: compile_server
compile_server: $(UTILS)/print_with_mutex.o $(UTILS)/base_config_parser.o $(SRV)/base_frame_sender.o $(SRV)/config_parser.o $(SRV)/frame_encoder.o $(SRV)/tcp_frame_sender.o $(SRV)/udp_frame_sender.o $(SRV)/viewer_synchronizer.o $(SRV)/frontend_server.o $(SRV)/main.o
	$(CXX) $(CXXFLAGS) $(SRV_LD) -o $(BIN)/sbc_server $^

$(SRV)/base_frame_sender.o: $(SRV)/base_frame_sender.cpp
	$(CXX) $(CXXFLAGS) -I$(SRV)/include -I$(UTILS)/include -c -o $@ $<

$(SRV)/config_parser.o: $(SRV)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I$(SRV)/include -I$(UTILS)/include -c -o $@ $<

$(SRV)/frame_encoder.o: $(SRV)/frame_encoder.cpp
	$(CXX) $(CXXFLAGS) -I$(SRV)/include -I$(UTILS)/include -I$(CV2_HDR) -c -o $@ $<

$(SRV)/tcp_frame_sender.o: $(SRV)/tcp_frame_sender.cpp
	$(CXX) $(CXXFLAGS) -I$(SRV)/include -I$(UTILS)/include -c -o $@ $<

$(SRV)/udp_frame_sender.o: $(SRV)/udp_frame_sender.cpp
	$(CXX) $(CXXFLAGS) -I$(SRV)/include -I$(UTILS)/include -c -o $@ $<

$(SRV)/viewer_synchronizer.o: $(SRV)/viewer_synchronizer.cpp
	$(CXX) $(CXXFLAGS) -I$(SRV)/include -I$(UTILS)/include -c -o $@ $<

$(SRV)/frontend_server.o: $(SRV)/frontend_server.cpp
	$(CXX) $(CXXFLAGS) -I$(SRV)/include -I$(UTILS)/include -c -o $@ $<

$(SRV)/main.o: $(SRV)/main.cpp
	$(CXX) $(CXXFLAGS) -I$(SRV)/include -I$(UTILS)/include -c -o $@ $<

# ディスプレイノード側プログラムをコンパイル
.PHONY: compile_client
compile_client: $(UTILS)/print_with_mutex.o $(UTILS)/base_config_parser.o $(CLI)/base_frame_receiver.o $(CLI)/config_parser.o $(CLI)/memory_checker.o $(CLI)/tcp_frame_receiver.o $(CLI)/udp_frame_receiver.o $(CLI)/frame_decoder.o $(CLI)/frame_viewer.o $(CLI)/display_client.o $(CLI)/main.o
	$(CXX) $(CXXFLAGS) $(CLI_LD) -o $(BIN)/sbc_client $^

$(CLI)/base_frame_receiver.o: $(CLI)/base_frame_receiver.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -I$(UTILS)/include -c -o $@ $<

$(CLI)/config_parser.o: $(CLI)/config_parser.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -I$(UTILS)/include -c -o $@ $<

$(CLI)/memory_checker.o: $(CLI)/memory_checker.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -c -o $@ $<

$(CLI)/tcp_frame_receiver.o: $(CLI)/tcp_frame_receiver.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -I$(UTILS)/include -c -o $@ $<

$(CLI)/udp_frame_receiver.o: $(CLI)/udp_frame_receiver.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -I$(UTILS)/include -c -o $@ $<

$(CLI)/frame_decoder.o: $(CLI)/frame_decoder.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -I$(UTILS)/include -I$(CV2_HDR) -c -o $@ $<

$(CLI)/frame_viewer.o: $(CLI)/frame_viewer.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -I$(UTILS)/include -I$(CV2_HDR) -c -o $@ $<

$(CLI)/display_client.o: $(CLI)/display_client.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -I$(UTILS)/include -c -o $@ $<

$(CLI)/main.o: $(CLI)/main.cpp
	$(CXX) $(CXXFLAGS) -I$(CLI)/include -I$(UTILS)/include -c -o $@ $<

# ヘッドノード側プログラムを起動
.PHONY: test_server
test_server:
	$(BIN)/sbc_server $(CONF)/server_config.json

# ディスプレイノード側プログラムを起動
.PHONY: test_client
test_client:
	$(BIN)/sbc_client $(CONF)/client_config.json

# 実行ファイルを全消去
.PHONY: clean
clean:
	rm -f $(BIN)/*
	rm -f $(SRV)/*.o
	rm -f $(CLI)/*.o
	rm -f $(UTILS)/*.o

