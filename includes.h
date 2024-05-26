/*
    Apie:
        Čia bus išvardijamos visos bibliotekos, variables ir funkcijos, kurios gali būti reikalingos skirtingiems failams.
*/
#pragma once
#define NOMINMAX //<_ windows.h overwritina cmath min ir max be šito
#include<Windows.h>

#include<ft2build.h>
#include FT_FREETYPE_H

#include<PyCLink/pyclink.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_syswm.h>

#include<iostream>
#include<cstring>
#include<fstream>
#include<vector>
#include<string>
#include<chrono>
#include<stdlib.h>
#include<cmath>
#include<algorithm>

//Global variables and structs
struct pixel_struct{
    int x;
    int y;
};
const char* WIND_TITLE = "Petto";
//x daugyba; _ dalyba
const double pi = 3.14159265359, pix2 = 6.28318530718, pi_2 = 1.57079632679, pix3_2 = 4.71238898038; 
//updates per second
int UPS = 144;
//ekrano dydis (kaip veiks kai daugiau nei vienas ekranas prijungtas?)
int WIDTH = GetSystemMetrics(SM_CXSCREEN), HEIGHT = GetSystemMetrics(SM_CYSCREEN);
//spalva, kurią langas visiškai ignoruos
SDL_Color COLOR_TO_IGNORE = {0,0,0,255};
//koks didžiausias šviesos skirtumas leistinas nuskaitant naują platformą
int platformScanColorME = 5;
//Pagrindiniai SDL variables
SDL_Window* wind;
SDL_Renderer *rend;
SDL_Event evt;
//window handle
HWND hWnd;
//Rect for text input box
SDL_Rect textRect;
//Pelės pozicija
int mx, my;
//Text box input
std::string textinput{};
FT_Library ft;
FT_FaceRec_* face;
int fontSize = 24;
SDL_Color colors[256];

//Visi platformų taškai
std::vector<int> platformPoints;

namespace petto{
    pixel_struct target_point; //kur nueiti;

    /**
     * 
    */
    void processCharacter();
    /**
     * 
    */
    void createCharacter();
}


//Defining functions (nesvarbus eiliškumas, tiesiog jų sąrašas, BET čia funkcijų neturi būti, jeigu jos neiškviečiamos iš kitų scriptų (pvz INITIALIZE nėra))
std::vector<int> GETSCREENGROUND(int colorME);
bool MakeWindowTranparent();
void startPlatformScanThread();
void altFunc();
void loadFonts();
void displayText(std::string sentence, SDL_Rect &textBox, int fontMaxHeight);
void chatGPTinquiry(std::vector<std::string> words);

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
    std::chrono::duration<double> timeDifference = std::chrono::duration_cast<std::chrono::seconds>(then-now);
    std::cout << "took to complete: " << timeDifference.count();
*/