# Makefile
CC = g++
CFLAGS = -I/usr/local/include/opencv2 -I/usr/local/include/opencv \
         -L/usr/local/lib64 \
         -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio
TARGET = main

# コンパイル
all: 
	$(CC) $(TARGET).cpp $(CFLAGS)

# バイナリ全消去
.PHONY: clean
clean:
	rm -f *.out

