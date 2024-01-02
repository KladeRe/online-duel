build:
	rm -rf main.o
	g++ -c main.cpp
	g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system

run:
	./sfml-app