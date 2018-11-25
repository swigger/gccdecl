CXX=g++
CXXFLAGS=-I$(shell $(CXX) -print-file-name=plugin)/include -fPIC -O0 -g 

all:gccdecl.so

gccdecl.so:gccdecl.o
	$(CXX) -shared -o $@ -fPIC $^

vpath %.cpp ../gccdecl
