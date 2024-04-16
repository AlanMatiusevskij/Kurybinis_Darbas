all:
	./mingw32/bin/i686-w64-mingw32-g++ -I source/include -L source/lib -g -o main main.cpp shapes.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf