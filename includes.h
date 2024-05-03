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
#include<fstream>
#include<vector>
#include<chrono>
#include<cmath>
#include<algorithm>

//Global variables and structs
struct pixel_struct{
    int x;
    int y;
};

const std::string WIND_TITLE{"Petto - your personal delight"};
const double pi = 3.14159265359, pix2 = 6.28318530718, pi_2 = 1.57079632679, pix3_2 = 4.71238898038; //x - daugyba; _ - dalyba
//updates per second
int UPS = 144;
//ekrano dydis (kaip veiks kai daugiau nei vienas ekranas prijungtas?)
int WIDTH = GetSystemMetrics(SM_CXSCREEN), HEIGHT = GetSystemMetrics(SM_CYSCREEN);
//spalva, kurią langas visiškai ignoruos
SDL_Color COLOR_TO_IGNORE = {255,0,255,255};
//koks didžiausias spalvų skirtumas leistinas prieš nuskaitant naują platformą
int platformScanColorME = 5;
//Pagrindiniai SDL variables
SDL_Window* wind;
SDL_Renderer *rend;
SDL_Event evt;
//window handle
HWND hWnd;

//Visi platformų taškai
std::vector<pixel_struct> platformPoints;

//Defining functions (nesvarbus eiliškumas, tiesiog jų sąrašas, BET čia funkcijų neturi būti, jeigu jos neiškviečiamos iš kitų scriptų (pvz INITIALIZE nėra))
std::vector<pixel_struct> GETSCREENGROUND(int colorME);
bool MakeWindowTranparent();
void createCharacterBones();
void startPlatformScanThread();
void presentBones();

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