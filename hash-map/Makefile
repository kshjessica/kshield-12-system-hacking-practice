CXX = clang++
CXXFLAGS = -Wall -Wno-unused-command-line-argument -fstack-protector -Wl,-z,relro,-z,now
TARGET = hash_map

SRC_DIR = src/
OBJ_DIR = obj/
BIN_DIR = bin/

DEPENDENCIES = cli object
OBJS = $(addprefix $(OBJ_DIR), command.o object.o array.o hash_map.o bucket.o)

.PHONY: all
all: init $(TARGET)

.PHONY: init
init:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

.PHONY: $(TARGET)
$(TARGET): $(DEPENDENCIES)
	$(CXX) $(SRC_DIR)cmd/main.cc $(OBJS) -o $(BIN_DIR)$(TARGET) $(CXXFLAGS)

.PHONY: cli
cli:
	$(CXX) $(SRC_DIR)cli/command.cc -c -o $(OBJ_DIR)command.o $(CXXFLAGS)

.PHONY: object
object:
	$(CXX) $(SRC_DIR)object/object.cc -c -o $(OBJ_DIR)object.o $(CXXFLAGS)
	$(CXX) $(SRC_DIR)object/array.cc -c -o $(OBJ_DIR)array.o $(CXXFLAGS)
	$(CXX) $(SRC_DIR)object/hash_map.cc -c -o $(OBJ_DIR)hash_map.o $(CXXFLAGS)
	$(CXX) $(SRC_DIR)object/bucket.cc -c -o $(OBJ_DIR)bucket.o $(CXXFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*
