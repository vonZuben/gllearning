all:
	make compile

compile:
	#export LIBS=-ldl make
	g++ -o prgm main.cpp gl3w.c libglfw3.a -lGL -lX11 -lXxf86vm -lXrandr -lXi -lpthread -lXcursor -lXinerama -ldl -Wall -g -O0
	#g++ -o prgm main.cpp -lGLEW -lglfw -lGL -Wall -g -O0

run: prgm
	./prgm

tags:
	ctags -R *

.PHONY: tags
