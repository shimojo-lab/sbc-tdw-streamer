# Makefile
TARGET = main
CC = g++
COPTIONS = -o $(TARGET).out -Wall -std=c++11
CXXFLAGS = -I /usr/local/include/opencv2 -I /usr/local/include/opencv \
           -L /usr/local/lib64 \
           -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_videoio \
		   -l boost_system -l pthread

# コンパイル
all:
	$(CC) $(COPTIONS) $(CXXFLAGS) $(TARGET).cpp

# バイナリを実行
test:
	./$(TARGET).out

# バイナリを全消去
.PHONY: clean
clean:
	rm -f $(TARGET).out

