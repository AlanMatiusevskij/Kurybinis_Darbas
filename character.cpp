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
    //y kuria puse suktis
    bool rotDir;
    //kaulo ilgis
    double length;
    //taškas, nuo kurio "ištiesiamas" kaulas; Matematinės funkcijos visada palaiko juos sujungtus.
    double x;
    double y;
};
enum{
    K_PEDA = 0,
    K_KELIS = 1,
    D_PEDA = 2,
    D_KELIS = 3,
    DUBUO = 4,
    JOINT_COUNT = 5
};
enum spriteIndexes{
    //Pirma eina kategorija, paskui kategorijoje esanciu sprite skaicius, tuomet is eiles sprite pavadinimai, tokiu tipu: kategorija_pavadinimas; 
    category_test = 0, categoryTest_SpriteNumb = 2, test_koja = 0, test_kunelis = 1,

    totalSpriteCategoryNumb = 1
};
//Išsaugoma 'SDL_Rect' pozicija ir 'SDL_Texture' sprite informacija kiekvienai nuotraukai. 
struct spriteInfo{
    SDL_Surface* img_data;
    SDL_Texture* img;
    SDL_Rect dimensions;//istrink, jeigu eina su New sDl rect
};
int feet_index[2] = {K_PEDA, D_PEDA};
int destinationME = 5;
double g = 0.05;
//sprites[kategorijos indeksas][kategorijoje esancios nuotraukos indeksas]
std::vector<std::vector<spriteInfo>> sprites;
std::vector<JOINT_STRUCT> joints;
//std::vector<SDL_Rect> spritePositions;

std::string collisions();
void updateBones();

/**
 * Funckija, sukurianti visus kaulus.
 * Kol kas rankiniu būdu reikia įrašyti jų informaciją.
*/
void createCharacterBones(){
    double angle;
    joints.resize(JOINT_COUNT);
    //-99 nurodo nenaudojamą reikšmę.
    joints[DUBUO] = {-99, -99, false, -99, (double)charStartingPos.x, (double)charStartingPos.y};

    joints[K_KELIS] = {angle, angle, true, kelisIkiKlubuIlgis, cosf(angle)*kelisIkiKlubuIlgis + joints[DUBUO].x, joints[DUBUO].y + sinf(angle)*kelisIkiKlubuIlgis};
    angle = 92*pi/180;
    joints[D_KELIS] = {angle, angle, false, kelisIkiKlubuIlgis, cosf(angle)*kelisIkiKlubuIlgis + joints[DUBUO].x, joints[DUBUO].y + sinf(angle)*kelisIkiKlubuIlgis};

    angle = 88*pi/180;
    joints[K_PEDA] = {angle, angle, true, kelisIkiPeduIlgis, cosf(angle)*kelisIkiPeduIlgis + joints[K_KELIS].x, joints[K_KELIS].y + sinf(angle)*kelisIkiPeduIlgis};
    angle = 92*pi/180;
    joints[D_PEDA] = {angle, angle, false, kelisIkiPeduIlgis, cosf(angle)*kelisIkiPeduIlgis + joints[D_KELIS].x, joints[D_KELIS].y + sinf(angle)*kelisIkiPeduIlgis};
    return;
}

void updateBones(){
    int org, dest;
    
    org = K_KELIS; dest = DUBUO;
    joints[org].x = cosf(joints[org].ANGLE)*joints[org].length + joints[dest].x;
    joints[org].y = joints[dest].y + sinf(joints[org].ANGLE)*joints[org].length;

    org = D_KELIS; dest = DUBUO;
    joints[org].x = cosf(joints[org].ANGLE)*joints[org].length + joints[dest].x;
    joints[org].y = joints[dest].y + sinf(joints[org].ANGLE)*joints[org].length;

    org = K_PEDA; dest = K_KELIS;
    joints[org].x = cosf(joints[org].ANGLE)*joints[org].length + joints[dest].x;
    joints[org].y = joints[dest].y + sinf(joints[org].ANGLE)*joints[org].length;

    org = D_PEDA; dest = D_KELIS;
    joints[org].x = cosf(joints[org].ANGLE)*joints[org].length + joints[dest].x;
    joints[org].y = joints[dest].y + sinf(joints[org].ANGLE)*joints[org].length;

    // spritePos[0][0].x = joints[K_KELIS].x - spritePos[0][0].w/2; spritePos[0][0].y = joints[K_KELIS].y+5;
    // spritePos[0][1].x = joints[D_KELIS].x- spritePos[0][1].w/2; spritePos[0][1].y = joints[D_KELIS].y+5;
    // spritePos[0][2].x = joints[DUBUO].x- spritePos[0][2].w/2; spritePos[0][2].y = joints[DUBUO].y;

    return;
}

/**
 * Simulates basic physical interactions.
*/
void physics(){
    std::string collision_tag = collisions();
    // std::cout << collision_tag <<"\n";
    //KOJOS, D_PEDA, K_PEDA;
    //FOR NOW ONLY IF HIT GROUND;
    if(collision_tag != "-"){
        velY = 0;
    }
    else{
        velY +=g;
    }

    return;
}

void presentBones(){
    SDL_SetRenderDrawColor(rend, 255,255,255,255);
    //SDL_SetRenderDrawColor(rend, 100,100,100,255);
    for(JOINT_STRUCT joint : joints)
        SDL_RenderDrawPoint(rend, joint.x, joint.y);
    SDL_RenderDrawLine(rend, joints[K_PEDA].x, joints[K_PEDA].y, joints[K_KELIS].x, joints[K_KELIS].y);
    SDL_RenderDrawLine(rend, joints[D_PEDA].x, joints[D_PEDA].y, joints[D_KELIS].x, joints[D_KELIS].y);

    SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, joints[K_KELIS].x, joints[K_KELIS].y);
    SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, joints[D_KELIS].x, joints[D_KELIS].y);
    return;
}

/**
 * Detects collisions.
 * Returns a tag, such as, "ground".
 * THINK OF A BETTER AND SMOOTHER AND MORE ACCURATE COLLISION DETECTION
*/
std::string collisions(){
    //check for ground
    int feet = 0;
    int whichFoot = -1;
    bool nextFoot = false;
    //DISGUSTING, THIS AINT WORKING. IS FLOOR FINDER EVEN WORKING PROPERLY?
    for(int i : feet_index){
        if(platformPoints.size() >= int(joints[i].y + 1)  *WIDTH -joints[i].x + WIDTH|| joints[i].y + 2 >= HEIGHT-1){
            if(platformPoints[int(joints[i].y + 1) *WIDTH -joints[i].x + WIDTH] == 1 || joints[i].y + 2 >= HEIGHT-1){
                whichFoot = i;
                feet++;
            }
        }
    }
    if(feet == 1){
        if(whichFoot == K_PEDA) return "K_PEDA";
        if(whichFoot == D_PEDA) return "D_PEDA";
        else std::cout << "KAZKAS NEGERAI CIA!";
    }
    else if(feet == 2) return "KOJOS";
    
    return "-"; //no collision
}
double angl(double degrees){
    return degrees*pi/180;
}

//this is a mess MAKE IT NOT A MESS
bool allowedToChangeState = true;
bool begin[5] = {true,true,true,true,true};
void resetPos(){
    int count= 0;
        //i was desperate, its 4am when im writing this
    int indx[4] = {D_KELIS, K_KELIS, D_PEDA, K_PEDA};
    for(int i : indx){
        if(std::abs(joints[i].defaultAngle-joints[i].ANGLE) > angl(1)){
            if(joints[i].ANGLE > joints[i].defaultAngle) joints[i].ANGLE -= angl(1);
            else joints[i].ANGLE += angl(1);
        }
        else{
            joints[i].ANGLE = joints[i].defaultAngle;
            count++;
        }
    }
    for(int i = 0; i < 5; i++) begin[i] = true;
    joints[K_KELIS].rotDir = true;
    joints[D_KELIS].rotDir = false;
    joints[K_PEDA].rotDir = true;
    joints[D_PEDA].rotDir = false;
    if(count == 4)
    allowedToChangeState = true;
    return;
}
void animate(){
    //Moving
    if(velX!=0){
        allowedToChangeState = false;
        double speed = velX;
        double A;
        int indx[2] = {D_KELIS, K_KELIS};
        for(int i : indx){
            if(joints[i].rotDir){
                joints[i].ANGLE -= speed/45;
                if(velX>0) A = 78;
                else A = 180-78;
                if(velX*joints[i].ANGLE <= angl(A) * velX) joints[i].rotDir = false;
            }
            else{
                joints[i].ANGLE+=speed/45;
                if(velX>0) A = 100;
                else A = 180-100;
                if(velX*joints[i].ANGLE >= angl(A)*velX) joints[i].rotDir = true;
            }
        }
        
        indx[0] = D_PEDA;
        indx[1] = K_PEDA;
        for(int i : indx){
            if(std::abs(joints[i+1].ANGLE - pi_2) < 0.01) begin[i] = true;
            if(joints[i].rotDir){
                begin[i] = false;
                joints[i].ANGLE -= speed/32;
                if(velX>0) A = 90;
                else A = 180-90;
                if(velX*joints[i].ANGLE <= angl(A)*velX) joints[i].rotDir = false;
            }
            else if(begin[i]){
                joints[i].ANGLE+=speed/32;
                if(velX>0) A = 110;
                else A = 180-110;
                if(velX*joints[i].ANGLE >= angl(A)*velX) joints[i].rotDir = true;
            }
        }
    }
    //Idle
    if(velX == 0 && velY == 0){
        resetPos();
    }
    return;
}

void prepareSprites(){
    SDL_Surface* tmpSurfc;
    sprites.resize(totalSpriteCategoryNumb);
    sprites[category_test].resize(categoryTest_SpriteNumb);

    tmpSurfc = SDL_LoadBMP("./assets/images/test/kojele.bmp");
    sprites[category_test][test_koja].img_data = tmpSurfc;
    sprites[category_test][test_koja].dimensions = {0, 0, tmpSurfc->w/kojuDydis, tmpSurfc->h/kojuDydis};
    sprites[category_test][test_koja].img = SDL_CreateTextureFromSurface(rend, tmpSurfc);
    SDL_FreeSurface(tmpSurfc); //free memory, kadangi vis tapatį variable naudojam, neturėtų būti memory leak, bet gali nutikti somehow vistiek 

    tmpSurfc = SDL_LoadBMP("./assets/images/test/kunelis.bmp");
    sprites[category_test][test_koja].img_data = tmpSurfc;
    sprites[category_test][test_kunelis].dimensions = {0,0, tmpSurfc->w/kunelioDydis, tmpSurfc->h/kunelioDydis};
    sprites[category_test][test_kunelis].img = SDL_CreateTextureFromSurface(rend, tmpSurfc);
    return;
}

void applySprites(){
    int flip;
    spriteInfo* pointer;
    //moving right - do NOT flip (based on provided sprites)
    if(velX >= 0)
        flip = 0;
    //moving left - DO flip
    if(velX < 0)
        flip = 1;

    pointer = &sprites[category_test][test_koja];
    SDL_RenderCopyEx(rend, pointer->img, NULL, new SDL_Rect{int(joints[K_KELIS].x - pointer->img_data->w/2), (int)joints[K_KELIS].y,pointer->img_data->w, pointer->img_data->h}, joints[K_KELIS].ANGLE*180/pi - 90, new SDL_Point{pointer->img_data->w/2, 0}, (SDL_RendererFlip)flip);
    
    pointer = &sprites[category_test][test_kunelis];
    SDL_RenderCopyEx(rend, pointer->img, NULL, new SDL_Rect{int(joints[DUBUO].x - pointer->img_data->w/2), (int)joints[DUBUO].y, pointer->img_data->w, pointer->img_data->h}, 0, new SDL_Point{0, 0}, (SDL_RendererFlip)flip);
    
    pointer = &sprites[category_test][test_koja];
    SDL_RenderCopyEx(rend, pointer->img, NULL, {int(joints[D_KELIS].x - pointer->img_data->w/2), (int)joints[D_KELIS].y, pointer->img_data->w, pointer->img_data->h}, joints[D_KELIS].ANGLE*180/pi - 90, new SDL_Point{pointer->img_data->w/2, 0}, (SDL_RendererFlip)flip);

    return;
}

//its literally not pathfinding rn
void pathfinding(){
    while(togoPoints.size() > 0){
        if(std::abs(togoPoints[0].x-joints[DUBUO].x) < destinationME)
            togoPoints.erase(togoPoints.begin());
        else break;
    }

    if(togoPoints.size()>0){
        if(togoPoints[0].x > joints[DUBUO].x) velX = 1;
        else velX = -1;
    }
    else velX = 0;
    return;
}

void debuglines(){
    SDL_SetRenderDrawColor(rend, 0,255,0,255);
    if(togoPoints.size()>0) SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, togoPoints[0].x, joints[DUBUO].y);
    return;
}

/**
 * Calls all functions for the character to work properly.
*/
void processCharacter(){
    physics();
    pathfinding();
    animate();

    joints[DUBUO].x += velX;
    joints[DUBUO].y += velY;
    updateBones();

    applySprites();
    presentBones();
    debuglines();
    return;
}