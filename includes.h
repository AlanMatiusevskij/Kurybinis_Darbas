/*
    Apie:
        Čia bus išvardijamos visos bibliotekos, variables ir funkcijos, kurios gali būti reikalingos skirtingiems failams.
*/
#pragma once
#define NOMINMAX //<_ windows.h overwritina cmath min ir max be šito
#include<Windows.h>

#include<ft2build.h>
#include FT_FREETYPE_H

#include<SDL2/SDL.h>
#include<SDL2/SDL_syswm.h>

#include<iostream>
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

const std::string WIND_TITLE{"Petto"};
//x daugyba; _ dalyba
const double pi = 3.14159265359, pix2 = 6.28318530718, pi_2 = 1.57079632679, pix3_2 = 4.71238898038; 
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
//Rect for text input box
SDL_Rect textRect;
//Pelės pozicija
int mx, my;
double velY = 0, velX = 0; //velocity
//Kai kurie veikėjo duomenys
pixel_struct charStartingPos = {400,100};
//Text box input
std::string textinput{};
FT_Library ft;
FT_FaceRec_* face;
int fontSize = 24;
SDL_Color colors[256];

enum kaulu_ilgiai{
    calf = 45,
    thigh = 65
};
int ground = calf + thigh; //ŠITAS TURI BŪTI ŠIEK TIEK ILGESNIS UŽ ATSTUMĄ NUO DUBENS IR KOJŲ GALŲ NUOTRAUKOS.
enum sprite_x_kartu_sumazintas_dydis{
    kojuDydis = 3,
    kunelioDydis = 4
};
enum{
    dideja = 1,
    mazeja = -1
};

//Visi platformų taškai
std::vector<int> platformPoints;
//Taskai, iki kurių nueiti.
std::vector<pixel_struct> togoPoints;

//Defining functions (nesvarbus eiliškumas, tiesiog jų sąrašas, BET čia funkcijų neturi būti, jeigu jos neiškviečiamos iš kitų scriptų (pvz INITIALIZE nėra))
std::vector<int> GETSCREENGROUND(int colorME);
bool MakeWindowTranparent();
void createCharacterBones();
void startPlatformScanThread();
void processCharacter();
void displayText();
void altFunc();
void loadFonts();
void displayText(std::string sentence, SDL_Rect &textBox);
double rad(double degrees);
void prepareSprites();
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