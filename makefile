# -*- coding: utf8 -*-

FILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT_DIR := $(shell dirname $(FILE_PATH))
PROJECT_NAME := $(notdir $(patsubst %/,%,$(dir $(FILE_PATH))))
BUILD_DIR ?= $(PROJECT_DIR)/staging
BIN_DIR ?= "$(BUILD_DIR)/Debug"
BUILD_MODE ?= Release # Either Debug or Release
BUILD_SHARED_LIBS ?= OFF
GRIND ?= valgrind
GRIND_OPTS ?= --show-leak-kinds=all --leak-check=full --track-origins=yes -v

default: all

debug:
	@echo "FILE_PATH: $(FILE_PATH)"
	@echo "PROJECT_DIR: $(PROJECT_DIR)"
	@echo "PROJECT_NAME: $(PROJECT_NAME)"
	@echo "BIN_DIR: $(BIN_DIR)"
	@echo "BUILD_DIR: $(BUILD_DIR)"
	@echo "BUILD_MODE: $(BUILD_MODE)"
	@echo "GRIND: $(GRIND)"
	@echo "SDKROOT: $(SDKROOT)"

clean:
	@rm -rf "$(BUILD_DIR)"

prepare:
	@mkdir -p "$(BUILD_DIR)"
	@cmake -B $(BUILD_DIR) -D CMAKE_BUILD_TYPE=${BUILD_MODE} \
		-D BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} -S $(PROJECT_DIR) \
		-D WITH_TOOLS=0 -D WITHOUT_PNG=1 -D JustOnce_WithTest=1

build:
	cmake --build "$(BUILD_DIR)"

run:
	$(BIN_DIR)/./JustOnceCMD -U -a "neo@matr.ix" -i "architect" -l 23 -d 7 -k "$(PROJECT_DIR)/Key.hash"

docs: clean-docs
	doxygen docs/doxygen.cfg > docs/doxygen.log 2> docs/doxygen.err.log

open-docs:
	open docs/html/index.html

clean-docs:
	rm -rf docs/{html,latex}
	rm -f docs/doxygen*.log

build-run: build run

all: prepare build-run