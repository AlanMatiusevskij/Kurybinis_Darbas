/**
 * @AlanMatiusevskij
 * @Gantzomara
*/
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
        SDL_GetGlobalMouseState(&mx, &my);
        SDL_SetRenderDrawColor(rend, COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b, COLOR_TO_IGNORE.a);
        SDL_RenderClear(rend);
        //Pradžia
        
        processCharacter();
        if(textInputReady) textInputFunctionallity();

        //Inputs
        SDL_PollEvent(&evt);
        if(GetAsyncKeyState(VK_LMENU)!=0) altFunc();
        if(evt.type == SDL_QUIT || evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_q && !textInputReady)
            break;
            
        //Pabaiga
        SDL_RenderPresent(rend);
        SDL_Delay((1/float(UPS))*1000);
    }
    FT_Done_FreeType(ft);
    SDL_Quit();
    return 0;
}

void INITIALIZE(){
    //FreeConsole();
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)) std::cout << "Failed to initialize SDL!\n";
    if(FT_Init_FreeType(&ft)) std::cout << "Failed to initialize FreeType library!\n";

    wind = SDL_CreateWindow(WIND_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP);
    rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(rend, SDL_BlendMode::SDL_BLENDMODE_NONE);

    //Load fonts
    SDL_StartTextInput();
    loadFonts();

    //Get window handle;
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(wind, &wmInfo);
    hWnd = wmInfo.info.win.window;
    MakeWindowTranparent();

    startPlatformScanThread();

    createCharacterBones();
    prepareSprites();
    return;
}