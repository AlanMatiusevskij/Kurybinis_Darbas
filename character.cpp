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
int feet_index[2] = {K_PEDA, D_PEDA};
int destinationME = 5;
double g = 0.05;

enum spriteIndexes{
    test = 0, test_koja = 0, test_kunelis = 1,

    totalSpriteCategoryNumb = 1
};
std::vector<std::vector<SDL_Texture*>> charSprites;
std::vector<std::vector<SDL_Rect>> spritePos;

std::string collisions();
void updateBones();

/**
 * Funckija, sukurianti visus kaulus.
 * Kol kas rankiniu būdu reikia įrašyti jų informaciją.
*/
void createCharacterBones(){
    double angle, length;
    joints.resize(JOINT_COUNT);
    //-99 nurodo nenaudojamą reikšmę.
    joints[DUBUO] = {-99, -99, false, -99, 1200 ,500};

    length = 80;
    angle = 88*pi/180; 
    joints[K_KELIS] = {angle, angle, true, length, cosf(angle)*length + joints[DUBUO].x, joints[DUBUO].y + sinf(angle)*length};
    angle = 92*pi/180;
    joints[D_KELIS] = {angle, angle, false, length, cosf(angle)*length + joints[DUBUO].x, joints[DUBUO].y + sinf(angle)*length};

    length = 32;
    angle = 88*pi/180;
    joints[K_PEDA] = {angle, angle, true, length, cosf(angle)*length + joints[K_KELIS].x, joints[K_KELIS].y + sinf(angle)*length};
    angle = 92*pi/180;
    joints[D_PEDA] = {angle, angle, false, length, cosf(angle)*length + joints[D_KELIS].x, joints[D_KELIS].y + sinf(angle)*length};
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

    spritePos[0][0].x = joints[K_KELIS].x - spritePos[0][0].w/2; spritePos[0][0].y = joints[K_KELIS].y+5;
    spritePos[0][1].x = joints[D_KELIS].x- spritePos[0][1].w/2; spritePos[0][1].y = joints[D_KELIS].y+5;
    spritePos[0][2].x = joints[DUBUO].x- spritePos[0][2].w/2; spritePos[0][2].y = joints[DUBUO].y;

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
        velY+=g;
    }

    return;
}

void presentBones(){
    SDL_SetRenderDrawColor(rend, 100,100,100,255);
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
        if(platformPoints.size() > WIDTH*HEIGHT - (round(joints[i].y) *WIDTH+round(joints[i].x) - WIDTH) ){
            if(platformPoints[WIDTH*HEIGHT - (round(joints[i].y) *WIDTH+round(joints[i].x) - WIDTH)] == 1 && platformPoints[WIDTH*HEIGHT - (round(joints[i].y) *WIDTH+round(joints[i].x) - WIDTH) + 1] == 1 && platformPoints[WIDTH*HEIGHT - (round(joints[i].y) *WIDTH+round(joints[i].x) - WIDTH)-1] == 1){
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
    //43, 37
    spritePos.resize(totalSpriteCategoryNumb);
    spritePos[0].push_back({0, 0, 86/3, 74/3}); //0
    spritePos[0].push_back({0, 0, 86/3, 74/3}); //1
    spritePos[0].push_back({0,0, 628/7, 716/7}); //2
    charSprites.resize(totalSpriteCategoryNumb);
    charSprites[0].resize(2);

    tmpSurfc = SDL_LoadBMP("./assets/images/test/kojele.bmp");
    charSprites[test][test_koja] = SDL_CreateTextureFromSurface(rend, tmpSurfc);

    tmpSurfc = SDL_LoadBMP("./assets/images/test/kunelis.bmp");
    charSprites[test][test_kunelis] = SDL_CreateTextureFromSurface(rend, tmpSurfc);
    return;
}

void applySprites(){
    //moving right
    if(velX >= 0){
        SDL_RenderCopyEx(rend, charSprites[test][test_koja], NULL, &spritePos[test][0], joints[K_KELIS].ANGLE*180/pi - 90, new SDL_Point{86/6, 74/6}, SDL_FLIP_NONE);
        SDL_RenderCopyEx(rend, charSprites[test][test_koja], NULL, &spritePos[test][1], joints[D_KELIS].ANGLE*180/pi - 90, new SDL_Point{86/6, 74/6}, SDL_FLIP_NONE);
        SDL_RenderCopyEx(rend, charSprites[test][test_kunelis], NULL, &spritePos[test][2], 0, new SDL_Point{0, 0}, SDL_FLIP_NONE);
    }
    //moving left
    if(velX < 0){
        SDL_RenderCopyEx(rend, charSprites[test][test_koja], NULL, &spritePos[test][0], joints[K_KELIS].ANGLE*180/pi - 90, new SDL_Point{86/6, 74/6}, SDL_FLIP_HORIZONTAL);
        SDL_RenderCopyEx(rend, charSprites[test][test_koja], NULL, &spritePos[test][1], joints[D_KELIS].ANGLE*180/pi - 90, new SDL_Point{86/6, 74/6}, SDL_FLIP_HORIZONTAL);
        SDL_RenderCopyEx(rend, charSprites[test][test_kunelis], NULL, &spritePos[test][2], 0, new SDL_Point{0, 0}, SDL_FLIP_HORIZONTAL);
    }
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