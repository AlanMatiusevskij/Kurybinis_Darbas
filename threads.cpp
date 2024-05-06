#include"includes.h"
/*
    Apie:
        Kodas atsakingas už multithreading.
    DĖMESIO:
        NĖ VIENA IŠ ČIA ESANČIŲ FUNKCIJŲ NETURĖTŲ BŪTI IŠKVIEČIAMOS RANKINIU BŪDU, *IŠSKYRUS* TOS, KURIOS PRASIDEDA 'start*' žodžiu!
*/

//Variables
SDL_TimerID platformTimer;

std::vector<SDL_Thread*> threads{};
std::vector<bool> threadRunning{};

//functions
int platformThreadFunc(void* data);
Uint32 updatePlatforms(Uint32 interval, void *data);

void startPlatformScanThread(){
    //kol 1 thread tik, sito uztenka
    threads.resize(1);
    threadRunning.push_back(false);
    platformTimer = SDL_AddTimer(500, updatePlatforms, nullptr);
    return;
}

Uint32 updatePlatforms(Uint32 interval, void *data){
    //for now manually assigned that threads[0]th is the thread used for platforms
    if(!threadRunning[0]){
        threadRunning[0] = true;
        threads[0] = SDL_CreateThread(platformThreadFunc, "platformthread", nullptr);
    }
    return interval;
}

int platformThreadFunc(void* data){
    std::vector<int> tmp;
    tmp = GETSCREENGROUND(platformScanColorME);
    platformPoints.clear();
    platformPoints = tmp;
    threadRunning[0] = false;
    return 0;
}