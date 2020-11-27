SHELL	:= /bin/bash
GEN_SRC := server.cpp	dkvs.cpp	store_types.cpp	store_constants.cpp
GEN_OBJ := $(patsubst	%.cpp,%.o,	$(GEN_SRC))

THRIFT_DIR := /home/cs557-inst/local/include
LIB_DIR := /home/cs557-inst/local/lib

INC := -I$(THRIFT_DIR) -Isrc/ -I$(THRIFT_DIR)/thrift

.PHONY: all clean

all:server

%.o:gen-cpp/%.cpp
	$(CXX)	-g	-std=c++17	-Wall	-DHAVE_INTTYPES_H	-DHAVE_NETINET_IN_H	$(INC)	-c	$<	-o	$@

server:server.o	$(GEN_OBJ)
	$(CXX)	-g $^	-o	$@	-std=c++17	-lstdc++	-L$(LIB_DIR)	-lthrift

client:
	g++	-g -std=c++17	-Wall	-DHAVE_INTTYPES_H	-DHAVE_NETINET_IN_H	-I/home/cs557-inst/local/include -Isrc/ -I/home/cs557-inst/local/include/thrift	-c	gen-cpp/client.cpp	-o	client.o
	g++	-g client.o	dkvs.o	store_types.o	store_constants.o	-o	client	-std=c++17	-L/home/cs557-inst/local/lib/	-lthrift

run:
	source	~/.bashrc && ./server 9090

start: all run

restart:clean all run

clean:
	$(RM)	*.o	server