#include"includes.h"
#include"windowscripts.cpp"

//Defining functions
void updatePlatforms();
void INITIALIZE();
Uint32 updatePlatforms(Uint32 interval, void *data);
int platformThreadFunc(void* data);

//Defining variables
SDL_TimerID platformTimer;

//MAGIJA prasideda nuo čia
int main(int argc, char *argv[]){
    INITIALIZE();
    while(true){
        SDL_SetRenderDrawColor(rend, COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b, COLOR_TO_IGNORE.a);
        SDL_RenderClear(rend);
        //Pradžia

        //Inputs
        SDL_PollEvent(&evt);
        if(evt.type == SDL_QUIT || evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_q)
            break;


        //Pabaiga
        SDL_RenderPresent(rend);
        SDL_Delay((1/float(UPS))*1000);
    }
    SDL_Quit();
    return 0;
}

void INITIALIZE(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    
    wind = SDL_CreateWindow(WIND_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP);
    rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED);

    //Get window handle;
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(wind, &wmInfo);
    hWnd = wmInfo.info.win.window;
    MakeWindowTranparent();

    //Start platform scanner thing
    platformTimer = SDL_AddTimer(500, updatePlatforms, nullptr);

    threads.resize(1);
    threadRunning.push_back(false);
    return;
}

/**
 * a function called every -interval- milliseconds; Creates a thread that calls the GETSCREENGROUND function.
 * SHOULDN'T BE MANUALLY CALLED
*/
Uint32 updatePlatforms(Uint32 interval, void *data){
    //for now manually assigned that threads[0]th is the thread used for platforms
    if(!threadRunning[0]){
        threadRunning[0] = true;
        threads[0] = SDL_CreateThread(platformThreadFunc, "platformthread", nullptr);
    }
    return interval;
}
/**
 * a thread function called by updatePlatforms(...); Calls the GETSCREENGROUND function.
 * SHOULDN'T BE MANUALLY CALLED
*/
int platformThreadFunc(void* data){
    platformPoints = GETSCREENGROUND(platformScanColorME);
    threadRunning[0] = false;
    return 0;
}




//little tiny note - did not copy others or ask AI to write code for this project at any time EXCEPT: making window partially invisible; quickly get and save screen bitmap data; 