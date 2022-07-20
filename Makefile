all: run

run: main.cxx utils.cxx
	g++ main.cxx -o run `root-config --libs --cflags`