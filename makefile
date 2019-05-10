a.out: main.cpp instruction.hpp
	g++ -std=c++11 main.cpp instruction.hpp
run: a.out
	./a.out
