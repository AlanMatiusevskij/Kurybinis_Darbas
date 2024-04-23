all:
	i686-w64-mingw32-g++ -I src/include -L src/lib -g -o main ragdoll.cpp -lmingw32 -lSDL2main -lSDL2