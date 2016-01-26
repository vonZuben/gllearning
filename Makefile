all:
	make compile

compile:
	g++ -o prgm main.cpp -lGLEW -lglfw -lGL -Wall -g -O0

run: prgm
	./prgm

tags:
	ctags -R *

.PHONY: tags
