#include"includes.h"
/*
    Apie:
        Visos funkcijos susijusios su veikėju. Variables laikome 'includes.h' faile, nes juos gali reikėti kitoms funkcijoms keisti
    Bugs:
        -
*/

//Variables and structs
struct JOINT_STRUCT{
    //dabartinis kampas TARP kaulų; RADS
    double ANGLE;
    //kampas TARP kaulų, kurį stengiasi palaikyti; RADS
    double defaultAngle;
    //kaulo ilgis
    double length;
    //taškas, nuo kurio "ištiesiamas" kaulas; Matematinės funkcijos visada palaiko juos sujungtus.
    double x;
    double y;
};
double distance; //tarp kaulų galų; Leidžia nustatyti kampą; Ar šito reikia?
std::vector<JOINT_STRUCT> joints;
enum{
    K_PEDA = 0,
    K_KELIS = 1,
    D_PEDA = 2,
    D_KELIS = 3,
    DUBUO = 4,
    JOINT_COUNT = 5
};

//Functions
//



/**
 * Funckija, sukurianti visus kaulus.
 * Kol kas rankiniu būdu reikia įrašyti jų informaciją.
*/
void createCharacterBones(){
    double angle, length;
    joints.resize(JOINT_COUNT);
    //-99 nurodo nenaudojamą reikšmę.
    joints[DUBUO] = {-99, -99, -99, 400 ,600};

    length = 50;
    angle = 100*pi/180; 
    joints[K_KELIS] = {angle, angle, length, cosf(angle)*length + joints[DUBUO].x, joints[DUBUO].y + sinf(angle)*length};
    angle = 92*pi/180;
    joints[D_KELIS] = {angle, angle, length, cosf(angle)*length + joints[DUBUO].x, joints[DUBUO].y + sinf(angle)*length};

    length = 60;
    angle = 90*pi/180;
    joints[K_PEDA] = {angle, angle, length, cosf(angle)*length + joints[K_KELIS].x, joints[K_KELIS].y + sinf(angle)*length};
    angle = 90*pi/180;
    joints[D_PEDA] = {angle, angle, length, cosf(angle)*length + joints[D_KELIS].x, joints[D_KELIS].y + sinf(angle)*length};
    return;
}

void physics(){
    
}

void presentBones(){
    SDL_SetRenderDrawColor(rend, 255,255,255,255);
    for(JOINT_STRUCT joint : joints)
        SDL_RenderDrawPoint(rend, joint.x, joint.y);
    SDL_RenderDrawLine(rend, joints[K_PEDA].x, joints[K_PEDA].y, joints[K_KELIS].x, joints[K_KELIS].y);
    SDL_RenderDrawLine(rend, joints[D_PEDA].x, joints[D_PEDA].y, joints[D_KELIS].x, joints[D_KELIS].y);

    SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, joints[K_KELIS].x, joints[K_KELIS].y);
    SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, joints[D_KELIS].x, joints[D_KELIS].y);
    return;
}

    //loads an image, creates its texture and creates a rect for it;
    // SDL_Surface* surfc= SDL_LoadBMP("./image.bmp");
    // SDL_Texture *legText;
    // legText=SDL_CreateTextureFromSurface(rend, surfc);
    // SDL_Rect img{500,500,200,250};

    //SDL_RenderCopyEx(rend, legText, NULL, &img, i, new SDL_Point{img.w-40, 0}, SDL_FLIP_NONE);