CC = clang
CCFLAGS = -Wall -fstack-protector -s -Wl,-z,relro,-z,now
TARGET = tiny_vi

SRC_DIR = src/
BIN_DIR = bin/

SRCS = $(notdir $(wildcard $(SRC_DIR)*.c))

all: init $(TARGET)

init:
	mkdir -p $(BIN_DIR)

$(TARGET): $(addprefix $(SRC_DIR), $(SRCS))
	$(CC) $^ -o $(BIN_DIR)$@ $(CCFLAGS)

clean:
	rm $(BIN_DIR)$(TARGET)