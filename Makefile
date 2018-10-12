# Makefile

CXX = g++
OBJS = pi_tdw_streamer
SRC = ./src
HEADER = ./include
BIN = ./bin
TARGET = $(BIN)/$(OBJS)
CV2_HEADER = /usr/local/include/opencv2
CXXFLAGS = -Wall -std=c++11
CXXLINKS = -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_videoio \
           -l boost_system -l pthread

# コンパイル
.PHONY: all
all: $(SRC)/video_demuxer.o $(SRC)/main.o 
	$(CXX) $(CXXFLAGS) $(CXXLINKS) -o $(TARGET) $^

$(SRC)/main.o: $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -I $(HEADER) -c -o $@ $<

$(SRC)/video_demuxer.o: $(SRC)/video_demuxer.cpp
	$(CXX) $(CXXFLAGS) -I $(HEADER) -I $(CV2_HEADER) -c -o $@ $<

# バイナリを実行
.PHONY: test
test:
	$(TARGET)

# バイナリを全消去
.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -f $(SRC)/*.o

