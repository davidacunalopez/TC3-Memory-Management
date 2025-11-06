CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = memory_manager
SOURCE = memory_manager.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET) $(TARGET).exe

test: $(TARGET)
	./$(TARGET) input.txt 0
	@echo "\n=== Prueba con Best-fit ==="
	./$(TARGET) input.txt 1
	@echo "\n=== Prueba con Worst-fit ==="
	./$(TARGET) input.txt 2

.PHONY: all clean test

