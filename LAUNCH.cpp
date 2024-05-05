#include"includes.h"

#include"windowscripts.cpp"
#include"threads.cpp"
#include"character.cpp"

//Defining functions
void INITIALIZE();

//Defining variables

//MAGIJA prasideda nuo čia
int main(int argc, char *argv[]){
    INITIALIZE();

    while(true){
        SDL_GetMouseState(&mx, &my);
        SDL_SetRenderDrawColor(rend, COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b, COLOR_TO_IGNORE.a);
        SDL_RenderClear(rend);
        //Pradžia
        
        presentBones();

        //Inputs
        SDL_PollEvent(&evt);
        if(evt.type == SDL_QUIT || evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_q)
            break;
        if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT)
        std::cout << "yes";
        SDL_RenderDrawLine(rend, 10, 10, 100, 10);
        SDL_RenderDrawLine(rend, 10, 11, 100, 11);
        SDL_RenderDrawLine(rend, 10, 12, 100, 12);
        SDL_RenderDrawLine(rend, 10, 13, 100, 13);
        SDL_RenderDrawLine(rend, 10, 14, 100, 14);

        //Pabaiga
        SDL_RenderPresent(rend);
        SDL_Delay((1/float(UPS))*1000);
    }
    SDL_Quit();
    return 0;
}

void INITIALIZE(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
    
    wind = SDL_CreateWindow(WIND_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP);
    rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED);

    //Get window handle;
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(wind, &wmInfo);
    hWnd = wmInfo.info.win.window;
    MakeWindowTranparent();

    //Start platform scanner thing    
    startPlatformScanThread();
    createCharacterBones();

    return;
}
//https://wiki.libsdl.org/SDL2/SDL_GetKeyboardState