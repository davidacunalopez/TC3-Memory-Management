CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = memory_manager
SOURCE = memory_manager.c
FS_TARGET = simple_fs
FS_SOURCE = simple_fs.c

all: $(TARGET) $(FS_TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

$(FS_TARGET): $(FS_SOURCE)
	$(CC) $(CFLAGS) -o $(FS_TARGET) $(FS_SOURCE)

clean:
	rm -f $(TARGET) $(TARGET).exe $(FS_TARGET) $(FS_TARGET).exe

test: $(TARGET)
	./$(TARGET) input.txt 0
	@echo "\n=== Prueba con Best-fit ==="
	./$(TARGET) input.txt 1
	@echo "\n=== Prueba con Worst-fit ==="
	./$(TARGET) input.txt 2

.PHONY: all clean test

