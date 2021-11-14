CXX=g++
CFLAGS=-g -DDEBUG -include ./tools/debug.hpp

SRC=$(wildcard *.cpp)
TARGET=$(patsubst %.cpp,%,$(SRC))

test: 1.cpp
	$(CXX) $(CFLAGS) -o $@ $<


clean:
	rm -rf $(TARGET)
