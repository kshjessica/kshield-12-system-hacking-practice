CC = clang
CFLAGS = -Wall -fstack-protector-all -s -Wl,-z,relro,-z,now

TARGET = account

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
SRCS := $(filter-out $(SRC_DIR)/main.c, $(SRCS))
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: init $(TARGET)

init:
	mkdir -p $(SRC_DIR) $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(SRC_DIR)/main.c $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^

$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*