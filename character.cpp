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
    //taškas, nuo kurio "ištiesiamas" kaulas; Matematinės funkcijos visada palaiko juos sujungtus.
    double x;
    double y;
};
double boneLength = 100;
double distance; //tarp kaulų galų; Leidžia nustatyti kampą; Ar šito reikia?
std::vector<JOINT_STRUCT> joints;

enum{
    K_PEDA = 0,
    K_KULNAS = 1,
    DUBUO = 2,

    JOINT_COUNT = 3
};

//Functions
//

/**
 * Funckija, sukurianti visus kaulus.
 * Kol kas rankiniu būdu reikia įrašyti jų informaciją.
 * Paeksperementuok ar negaletum kampu reiksmiu pakeisti y logiskesnes
*/
void createCharacterBones(){
    joints.resize(JOINT_COUNT);
    double angle, tmpAngle;

    joints[K_PEDA] = {-99, -99, 500, 500};
    angle = 160*pi/180; tmpAngle = (pi-angle)/2;
    joints[K_KULNAS] = {angle, angle, cosf(tmpAngle)*boneLength + joints[K_PEDA].x, joints[K_PEDA].y - sinf(tmpAngle)*boneLength};
    joints[DUBUO] = {-99, -99, joints[K_KULNAS].x + cosf(pix2 - tmpAngle)*boneLength, joints[K_KULNAS].y - sinf(pix2 - tmpAngle)*boneLength};

    //joints[1] = {cosf(calcAngle +aleArmAngle)*armLength + joints[0].x, joints[0].y-sinf(calcAngle + aleArmAngle)*armLength};
    //joints[2] = {cosf(2*pi-calcAngle + aleArmAngle)*armLength + joints[1].x, joints[1].y-sinf(2*pi-calcAngle + aleArmAngle)*armLength};

    return;
}

void presentBones(){
    SDL_SetRenderDrawColor(rend, 255,255,255,255);
    for(JOINT_STRUCT joint : joints)
        SDL_RenderDrawPoint(rend, joint.x, joint.y);
    for(int i = 1; i < joints.size(); i++)
        SDL_RenderDrawLine(rend, joints[i].x, joints[i].y, joints[i-1].x, joints[i-1].y);
    return;
}


    //loads an image, creates its texture and creates a rect for it;
    // SDL_Surface* surfc= SDL_LoadBMP("./image.bmp");
    // SDL_Texture *legText;
    // legText=SDL_CreateTextureFromSurface(rend, surfc);
    // SDL_Rect img{500,500,200,250};

    //SDL_RenderCopyEx(rend, legText, NULL, &img, i, new SDL_Point{img.w-40, 0}, SDL_FLIP_NONE);