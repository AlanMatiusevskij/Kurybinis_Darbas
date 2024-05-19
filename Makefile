all:
	i686-w64-mingw32-g++ -I src/include -L src/lib -g -o Petto LAUNCH.cpp -lmingw32 -lSDL2main -lSDL2 -lgdi32 -lfreetype ./src/include/PyC-Link/pyclink.cpp