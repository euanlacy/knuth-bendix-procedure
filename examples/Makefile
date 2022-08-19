BUILD_PATH ?= .
BIN_PATH ?= .

vpath % ${BIN_PATH}

SRCS = ${wildcard *.cpp}

all: ${SRCS:%.cpp=%}

%: %.cpp
	$(CXX) ${CXXFLAGS} -I../include -I../third_party -o ${BIN_PATH}/$@ $^

