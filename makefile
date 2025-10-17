# compiler and flags
CXX = g++
CXX_FLAGS = -Wall -Wextra -Werror -O2

# 
REMOVE = rm -rf

# Targets Especified in the requirements

TARGETS = upload findrec seek1 seek2


#Directories path
SRC_DIR = src
OUT_DIR = out
INCLUDE_DIR = include
OBJ_DIR = obj
DATA_DIR = data
BIN_DIR = bin

# Default target
.PHONY: all build clean docker-build docker-run-upload docker-run-findrec docker-run-seek1 docker-run-seek2 help

all: build

clean:
	@echo "Cleaning program..."
	REMOVE out
	REMOVE obj
	REMOVE data
