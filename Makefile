.PHONY: all clean test

CC=gcc
BUILD=build
EXE=csvreader

NO_OP=1>/dev/null 2>/dev/null

all: build
	@echo done

test: build
	$(BUILD)/$(EXE) example/1.csv $(NO_OP)
	@echo OK
	test `$(BUILD)/$(EXE) example/zero.csv $(NO_OP) || echo $$?` = 33
	@echo OK
	test `$(BUILD)/$(EXE) $(NO_OP) || echo $$?` = 22
	@echo OK
	test `$(BUILD)/$(EXE) example/wrfmt.csv $(NO_OP) || echo $$?` = 22
	@echo OK

build: src/*.c
	mkdir -p $(BUILD)
	$(CC) $^ -o $(BUILD)/$(EXE)

clean:
	rm -rf $(BUILD)
