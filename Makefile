CXX= g++
CXXFLAGS= -g -W -Wall -Werror

all: myshell.cc
	$(CXX) myshell.cc $(CXXFLAGS) -o myshell

clean: myshell.cc
	rm *~ *.o
