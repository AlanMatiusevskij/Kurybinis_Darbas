/*
    Apie:
        Čia bus išvardijamos visos bibliotekos, variables ir funkcijos, kurios gali būti reikalingos skirtingiems failams.
*/
#pragma once
#define NOMINMAX //<_ cmath min ir max overwritina windows.h be šito
#include<Windows.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_syswm.h>

#include<iostream>
#include<vector>
#include<chrono>
#include<cmath>
#include<algorithm>

//Global variables
const std::string WIND_TITLE{"Petto - your personal delight"};
int UPS = 20;
int WIDTH = GetSystemMetrics(SM_CXSCREEN), HEIGHT = GetSystemMetrics(SM_CYSCREEN);
SDL_Color COLOR_TO_IGNORE = {255,0,255,255};
int platformScanColorME = 5;
SDL_Window* wind;
SDL_Renderer *rend;
SDL_Event evt;
HWND hWnd;

//A timer that constantly updates platform positions
SDL_TimerID platformTimer;
std::vector<SDL_Thread*> threads{};
std::vector<bool> threadRunning{};
struct pixel_struct{
    int x;
    int y;
};
std::vector<pixel_struct> platformPoints;

//Defining functions
std::vector<pixel_struct> GETSCREENGROUND(int colorME);
bool MakeWindowTranparent();
void displayFloors();



//some useful stuff:
    // std::cout << ("\033[31mThis is red font.\033[0m") << "\n";
    // std::cout << ("\033[32mThis is green font.\033[0m")<< "\n";
    // std::cout << ("\033[33mThis is yellow font.\033[0m")<< "\n";
    // std::cout << ("\033[34mThis is orange font.\033[0m THIS IS WHITE")<< "\n";
    //37 is white

    /* time difference with chrono:
    std::chrono::steady_clock::time_point now;
    std::chrono::steady_clock::time_point then;
    now = std::chrono::steady_clock::now();
    then = std::chrono::steady_clock::now();
    std::chrono::duration<double> timespan = std::chrono::duration_cast<std::chrono::seconds>(then-now);
    std::cout << "took to complete: " << timespan.count();
*/