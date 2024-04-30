#include"includes.h"

//Main variables
const std::string WIND_TITLE{"PC_PET"};
int WIDTH = GetSystemMetrics(SM_CXSCREEN), HEIGHT = GetSystemMetrics(SM_CYSCREEN);

//Defining functions
void INITIALIZE(SDL_Window*wind, SDL_Renderer *rend);

//Prasideda viskas nuo čia
int main(int argc, char *argv[]){
    SDL_Window *wind;
    SDL_Renderer *rend;
    SDL_Event evt;

    INITIALIZE(wind, rend);

    while(true){
        SDL_PollEvent(&evt);
        if(evt.type == SDL_QUIT)
            break;
    }

    SDL_Quit();
    return 0;
}

void INITIALIZE(SDL_Window*wind, SDL_Renderer *rend){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    wind = SDL_CreateWindow(WIND_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP);
    rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED);

    return;
}