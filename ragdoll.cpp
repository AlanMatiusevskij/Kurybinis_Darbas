#include<SDL2/SDL.h>
#include<iostream>
#include<vector>
#include<fstream>
#include<chrono>
#include<cmath>
#include<algorithm>

const int WIDTH = 1500, HEIGHT = 600, UPS = 144;
const double pi = 3.14159265359; 
double h_pi = pi/2, pi3_2 = pi * 3 /2, pi2 = 2*pi;
struct pos_struct{
    int x1;
    int y1;
    int x2;
    int y2;
};
struct point_struct{
    int x, y;
};
double angleSpeed = 0;
double defaultAngle = 160*pi/180;
double angleEM = 0.1;
bool ALREADYLANDED = false;
bool ONGROUND = false;
double general_velocityY = 0, general_velocityX = 0;
double drink = 1.6; //idk
double g = 0.015;
SDL_Event evt;
std::vector<pos_struct> ground;
int mx, my;
int startX, endX;

double distance = 50; // distance between hand and shoulder;
double armLength = 70;
struct joint_struct{
    double x, y;
    double maxAngle;
};
std::vector<joint_struct> joints;
struct{
    point_struct point;
    double MASSLEFT, MASSRIGHT;
}balance;
SDL_Rect angleNotch{0,0, 10, 10}; //x, y, w, h
SDL_Rect armLengthNotch{0,0, 10, 10};
SDL_Rect distanceNotch{0,0, 10, 10};
bool notchMoving[3]{false, false, false}; 
double ANGLE = 160*pi/180;//in rads
double aleArmAngle = h_pi;
//checks if mouse is on a button (an SDL_Rect);
bool onButton(SDL_Rect &rect){
    if(mx >= rect.x && mx <rect.x+rect.w&& my >= rect.y && my < rect.y+rect.h) 
        return true;
    return false;
}
void slider(SDL_Renderer* renderer, SDL_Rect &rect, double &value, double start_value, double end_value, bool &moving, int x, int y, int w, int bar_thickness){
    //check if notch pressed
    if(onButton(rect))
        if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT){
            startX = mx;
            moving = true;
        }
    //value logic if mouse moving and notch pressed
    if(evt.type == SDL_MOUSEMOTION && moving){
        endX = mx;
        value = (endX - startX) * (end_value -start_value) / w + value;
        startX = mx;
    }
    //check if notch unpressed
    if(evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LEFT)
        moving = false;

    //logical notch positions
    rect.x = x + (value - start_value )/(end_value - start_value) * w;
    rect.y = y - rect.h/2 + bar_thickness - 1;

    //value constraints
    if(rect.x < x){
        rect.x = x;
        value = start_value;
    }
    else if(rect.x > x + w){
        rect.x = x + w;
        value = end_value;
    }

    //Draw the slider;
    for(int i = 0; i < bar_thickness; i++)
        SDL_RenderDrawLine(renderer, x, y + i, x + w, y + i);
    SDL_RenderDrawRect(renderer, &rect);
    return;
}

double calcAngle;
int k = -1;
void HAND(){
    //I want the angle betweenthe one between 2 bones;, but formula uses a different one; ANgle/2 seems to give the one i want
    calcAngle = (pi-ANGLE)/2*k; //k defines direction
    std::cout << calcAngle*180/pi <<", " << ANGLE*180/pi<<"\n";
    joints[1] = {cosf(calcAngle +aleArmAngle)*armLength + joints[0].x, joints[0].y-sinf(calcAngle + aleArmAngle)*armLength};
    joints[2] = {cosf(2*pi-calcAngle + aleArmAngle)*armLength + joints[1].x, joints[1].y-sinf(2*pi-calcAngle + aleArmAngle)*armLength};
    return;
}
void DRAWHANDS(SDL_Renderer *rend){
    for(joint_struct joint : joints)
        SDL_RenderDrawPoint(rend, joint.x, joint.y);
    for(int i = 1; i < joints.size(); i++)
        SDL_RenderDrawLine(rend, joints[i].x, joints[i].y, joints[i-1].x, joints[i-1].y);
    return;
}

//iterates through ground and objects(items, like popcorn) vectors
int PEM = 2;
void collisionDetection(){
    for(pos_struct floor : ground){
        //feet is 0th
        if(floor.x1 <= joints[0].x && floor.x2 >= joints[0].x && std::abs(joints[0].y - floor.y1) < PEM)
            ONGROUND = true;
        else{
            ALREADYLANDED = true;
            ONGROUND = false;
        }
    }
}
double maxAngleSpeed = 0.03;
double angleFallenTo;
void balanceJumps(){
    // angleSpeed +=
    // angleSpeed +=

    return;
}

void angleRotation(){
    if(false)
        balanceJumps();
    else {
        angleSpeed+=(defaultAngle-ANGLE)/1500;
        angleSpeed+=(ANGLE-defaultAngle)/2500;
        //if(std::abs(ANGLE) >= pi) angleSpeed = -1 * angleSpeed / 1.1;
        if(std::abs(angleSpeed) > maxAngleSpeed) angleSpeed = maxAngleSpeed *(std::abs(angleSpeed)/angleSpeed);
    }
    return;
}

void PHYSICS(){
    if(!ONGROUND)
        general_velocityY+=g;
    if(ONGROUND && ALREADYLANDED){
        angleFallenTo = ANGLE;

        general_velocityY = -1*general_velocityY/drink;
        ALREADYLANDED = false;
        general_velocityX/=drink;
        if(std::abs(joints[0].y - ground[0].y1) < PEM-1) general_velocityX += cosf( h_pi+calcAngle)*armLength / 100;
        angleSpeed-=general_velocityY*(g+0.01) *(calcAngle/std::abs(calcAngle))*k*(-1) * drink;
    }

    joints[0].y += general_velocityY;
    joints[0].x += general_velocityX;

    return;
}
void checkForNotch(){
    for(bool check : notchMoving)
        if(check){
            if(armLength != 0 && distance/(2*armLength) <= 1 && distance/(2*armLength) >= -1) ANGLE =pi - 2*acosf(distance/(2*armLength));
            std::cout << ANGLE*180/pi << "\n";
            return;
        }
    PHYSICS();
    return;
}
void getBalancePoints(){
    //has a seperate list that holds feet or smt?
    balance.point.x = joints[0].x;
    balance.point.y = joints[0].y;
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow("ragdoll.cpp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIDTH, HEIGHT, 0);
    SDL_Renderer* rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ANGLE = 2*acosf(distance/(2*armLength));
    ground.push_back({0, HEIGHT*8/10, WIDTH-1, HEIGHT*8/10});
    joints.push_back({700, 300, -pi});

    joints.push_back({cosf(ANGLE)*armLength + 700, 300-sinf(ANGLE)*armLength,-1});
    joints.push_back({700, 300-sinf(ANGLE)*armLength, -1});

    while(true){
        SDL_GetMouseState(&mx, &my);
        SDL_PollEvent(&evt);
        if(evt.type == SDL_QUIT || evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_q)
            break;
        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 255,255,255,255);
        SDL_RenderDrawLine(rend, ground[0].x1, ground[0].y1, ground[0].x2, ground[0].y2);

        slider(rend, armLengthNotch, armLength, 0.0001, 400, notchMoving[0], 30,50,200,2);
        slider(rend, angleNotch, aleArmAngle, 0, 2*pi, notchMoving[1], 30,30,200,2);
        slider(rend, distanceNotch, distance, 0, 2*armLength, notchMoving[2], 30,70,200,2);
        //std::cout << "elbowAngle: " <<ANGLE * 360 / pi << ", distance: " << distance << ", ArmAngle: " << aleArmAngle*180/pi << ", armLength: " << armLength << "\n";
        
        getBalancePoints();
        
        HAND();
        DRAWHANDS(rend);
        
        collisionDetection();
        checkForNotch();

        if(std::abs(ANGLE - defaultAngle) > angleEM) angleRotation();
        else angleSpeed/=1.05;
        ANGLE+=angleSpeed;

        SDL_RenderPresent(rend);
        SDL_Delay((1/float(UPS))*1000);
    }
    SDL_Quit();
    return 0;
}