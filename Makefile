.PHONY: all clean test

CC=gcc
BUILD=build
EXE=csvreader

NO_OP=1>/dev/null 2>/dev/null

all: build
	@echo done

test: build
	$(BUILD)/$(EXE) example/1.csv $(NO_OP)
	test `$(BUILD)/$(EXE) example/zero.csv $(NO_OP) || echo $$?` = 33

build: src/*.c
	mkdir -p $(BUILD)
	$(CC) $^ -o $(BUILD)/$(EXE)

clean:
	rm -rf $(BUILD)
