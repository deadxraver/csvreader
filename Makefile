.PHONY: all clean

CC=gcc
BUILD=build
EXE=csvreader

all: build
	@echo done

build: src/*.c
	mkdir -p $(BUILD)
	$(CC) $^ -o $(BUILD)/$(EXE)

clean:
	rm -rf $(BUILD)
