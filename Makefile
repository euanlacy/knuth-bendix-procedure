BUILD_DIR := build
BIN_DIR := ${BUILD_DIR}/bin

export CXX := g++
export CXXFLAGS := -g -O3 -DNDEBUG -Wall -Wextra --std=c++20

vpath %.o ${BUILD_DIR}

all: test bench examples

${BUILD_DIR}:
	mkdir -p ${BUILD_DIR}

${BIN_DIR}:
	mkdir -p ${BIN_DIR}

tests: export BUILD_PATH = ../${BUILD_DIR}
tests: export BIN_PATH = ../${BIN_DIR}
tests:
	cd tests && ${MAKE}

bench: export BUILD_PATH = ../${BUILD_DIR}
bench: export BIN_PATH = ../${BIN_DIR}
bench: compile
	cd src && ${MAKE} bench
	cd benchmarks && ${MAKE}

examples: export BUILD_PATH = ../${BUILD_DIR}
examples: export BIN_PATH = ../${BIN_DIR}
examples: ${BUILD_DIR} ${BIN_DIR}
	cd examples && ${MAKE}

.PHONY: all tests clean examples

clean:
	rm -r ${BUILD_DIR} ${BIN_DIR}
