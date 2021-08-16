# -*- coding: utf8 -*-

FILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT_DIR := $(shell dirname $(FILE_PATH))
PROJECT_NAME := $(notdir $(patsubst %/,%,$(dir $(FILE_PATH))))
BUILD_DIR := $(PROJECT_DIR)/staging
WITH_TEST := 0
TEST_FLAGS :=  -D JustOnce_WithTest=0 \
	-D ShaOne_WithTest=0 \
	-D Testly_WithTest=0
BUILD_MODE = Release # Either Debug or Release
GRIND = valgrind
GRIND_OPTS = --show-leak-kinds=all --leak-check=full --track-origins=yes -v

default: all

debug:
	@echo "FILE_PATH: $(FILE_PATH)"
	@echo "PROJECT_DIR: $(PROJECT_DIR)"
	@echo "PROJECT_NAME: $(PROJECT_NAME)"

clean:
	@rm -rf "$(BUILD_DIR)"

prepare:
	@mkdir -p "$(BUILD_DIR)"
	@(cd $(BUILD_DIR) && cmake ${TEST_FLAGS} -D CMAKE_BUILD_TYPE=${BUILD_MODE} ..)

build:
	@make -C "$(BUILD_DIR)"

run:
	"$(BUILD_DIR)"/./JustOnceCMD -U -a "neo@matr.ix" -i "architect" -l 23 -d 7 < Key.hash

docs: clean-docs
	doxygen docs/doxygen.cfg > docs/doxygen.log 2> docs/doxygen.err.log

open-docs:
	open docs/html/index.html

clean-docs:
	rm -rf docs/{html,latex}
	rm -f docs/doxygen*.log

build-run: build run

all: prepare build-run