BUILD_PATH ?= .
BIN_PATH ?= .

vpath % ${BIN_PATH}
vpath %.o ${BUILD_PATH}

SRCS = ${wildcard *.cpp}

all: catch2.o test_knuth_bendix

catch2.o: catch2.cpp
	${CXX} ${CXXFLAGS} -c -o ${BUILD_PATH}/$@ $^

test_knuth_bendix: test_knuth_bendix.cpp
	${CXX} ${CXXFLAGS} -I../include -I../third_party -o ${BIN_PATH}/$@ ${BUILD_PATH}/catch2.o $^

