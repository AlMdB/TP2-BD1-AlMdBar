# compiler and flags
CXX = g++
CXX_FLAGS = -Wall -Wextra -Werror -O2

# 
REMOVE = rm -rf

# Targets Especified in the requirements

TARGETS = upload findrec seek1 seek2

UTEST = test-fileReader


#Directories path
SRC_DIR = src
OUT_DIR = out
INCLUDE_DIR = include
OBJ_DIR = obj
DATA_DIR = data
BIN_DIR = bin
TEST_DIR = test

DISPOSABLES = BIN_DIR DATA_DIR OBJ_DIR TEST_DIR OUT_DIR


SOURCES = $(wildcard $(SRC_DIR)/**/*.cpp)
HEADER = $(wildcard $(INCLUDE_DIR)/**/*.hpp)
# Default target
.PHONY: all build clean docker-build docker-run-upload docker-run-findrec docker-run-seek1 docker-run-seek2 help

all: build

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

$(TEST_DIR):
	@mkdir -p $(TEST_DIR)

build: $(BIN_DIR) $(OUT_DIR) $(patsubst %, $(BIN_DIR)/%, $(TARGETS))
	@echo "Build complete"

docker-build:

build-test: $(TEST_DIR)

clean:
	@echo "Cleaning program..."
	$(REMOVE) $(DISPOSABLEs)

test-fileReader: $(SRC_DIR)/FileReader.cpp $(INCLUDE_DIR)/FileReader.hpp | $(BIN_DIR)
	@echo "Creating fileReader for tests"
	$(CXX) $(CXX_FLAGS) -o $@ $<

