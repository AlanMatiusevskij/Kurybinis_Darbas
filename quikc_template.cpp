#include<iostream>
#include<SDL2/SDL.h>

const int WIDTH = 800, HEIGHT = 600;
int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIDTH, HEIGHT, 0);
    SDL_Event windowEvent;
    while(true){
        if(SDL_PollEvent(&windowEvent)){
            if(SDL_QUIT == windowEvent.type)
            break;
        }
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}

/*
how to execute (at new line press 'enter):
mingw32-make
./main
*/