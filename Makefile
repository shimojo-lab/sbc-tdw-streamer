# Makefile
TARGET = main
CC = g++
COPTIONS = -o $(TARGET).out -Wall
CFLAGS = -I /usr/local/include/opencv2 -I /usr/local/include/opencv \
         -L /usr/local/lib64 \
         -l opencv_core -l opencv_imgcodecs -l opencv_highgui -l opencv_videoio

# コンパイル
all:
	$(CC) $(COPTIONS) $(TARGET).cpp $(CFLAGS)

# バイナリを実行
test:
	./$(TARGET).out

# バイナリを全消去
.PHONY: clean
clean:
	rm -f $(TARGET).out

