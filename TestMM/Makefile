CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = test_util.c test_mm.c main.c
TARGET = test_mm

all: buddy

buddy: MM_SRC = buddy.c
buddy: $(SRCS) buddy.c
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) buddy.c

bitmap: MM_SRC = bitmap.c
bitmap: $(SRCS) bitmap.c
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) bitmap.c

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean buddy bitmap
