#include<iostream>
#include<vector>
#include<SDL2/SDL.h>

#include<SDL2/SDL_syswm.h>
#include <Windows.h>

int WIDTH = GetSystemMetrics(SM_CXSCREEN), HEIGHT = GetSystemMetrics(SM_CYSCREEN);
int UPS = 60;
SDL_Color COLOR_TO_IGNORE = {255,0,255,255};//bright magenta

struct point{
    int x1, y1, x2, y2;
};
std::vector<point> POINTS;

bool MakeWindowTranparent(SDL_Window*window){
    COLORREF colorKey = RGB(COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b);

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hWnd = wmInfo.info.win.window;

    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow("Floor finder", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event windowEvent;

    MakeWindowTranparent(window);
    while(true){
        SDL_SetRenderDrawColor(rend, COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b, COLOR_TO_IGNORE.a);
        SDL_RenderClear(rend);
        SDL_SetRenderDrawColor(rend, 255,0,0,0);

        if(SDL_PollEvent(&windowEvent)){
            if(SDL_QUIT == windowEvent.type || windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_q)
            break;
        }

        SDL_RenderPresent(rend);
        SDL_Delay((1/float(UPS))*1000);
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}
