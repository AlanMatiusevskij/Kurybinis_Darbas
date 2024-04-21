#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<SDL2/SDL.h>

const float pi = 3.14159265359;
      float h_pi = pi/2, pi3_2 = pi * 3 / 2;
const int WIDTH = 1500, HEIGHT = 800, UPS = 80;
double g = 0.1, airfriction = 0.01;

struct position_var{
    double x1, x2, y1, y2;
};
struct pixel_var{
    double x, y;
};
struct branch_struct{
    int length = 40;
    position_var pos;
    double vel = 0;
};
struct joints{
    //minMax angles aisku, Default angle tai kampas kury bandys palaikyti, delta_angle tai slight movements leidziami prie default angle, kad atrodytu kad siubuoja biski vietoje
    double minAngle, maxAngle, default_angle, delta_angle;
    branch_struct bone;
};
double main_acc = 0;
pixel_var lastPos;
std::vector<joints> branch;
pixel_var BASE;
int lastStatic = -1; //static branches are first ones in the list

enum circle_parts{
    I = 1,
    II = 2,
    III = 3,
    IV = 4
};
enum structure_names{
    STICKMANAS = 1,
};

//Didelis klausimas ar cia efficient taip daryti
struct pixel_info{
    //int state = 0; //nzn ar reik?
    pixel_var coord;
};
std::vector<std::vector<pixel_info>> objects;
double offset = 12 * pi/180; //max delta angle+ 2
double restoreSpeed = 0.1;

void drawLines(SDL_Renderer**rend){
    for(joints item : branch)
        SDL_RenderDrawLine(*rend, roundf(item.bone.pos.x1), roundf(item.bone.pos.y1), roundf(item.bone.pos.x2), roundf(item.bone.pos.y2));

    return;
}

bool inCirclePos(int index, position_var *pos){
    if(index == I){
        if(pos->x2 > pos->x1 && pos->y2 < pos->y1)  return true;
    }
    else if(index == II){
        if(pos->x2 < pos->x1 && pos->y2 < pos->y1)  return true; 
    }
    else if(index == III){
        if(pos->x2 < pos->x1 && pos->y2 > pos->y1)  return true;
    }
    else if(index == IV){
        if(pos->x2 > pos->x1 && pos->y2 > pos->y1)  return true;
    }

    return false;
}
//5,6 - leg indexes
bool isFalling(){
    //the legs quickly give in to the air friction and they go upwards
    for(std::vector<pixel_info> vector : objects){
        for(pixel_info data : vector){
            if( data.coord.y > std::round(branch[0].bone.pos.y1) && data.coord.x != std::round(branch[0].bone.pos.x1) 
                &&
                data.coord.y > std::round(branch[5].bone.pos.y2) && data.coord.x != std::round(branch[5].bone.pos.x2)
                &&
                data.coord.y > std::round(branch[6].bone.pos.y2) && data.coord.x != std::round(branch[6].bone.pos.x2)
            ) return true;
        }
    }
    return false;
}

// int hitGround = -1;
// double deltax, deltay;
// bool fell = false, rose = false;
// void simulation(){
//     if(isFalling()){ //if in free fall
//         //The very first bone will drag along the whole body
//         //acceleration will be gained from the fall
//         //it will affect the speed of bone rotation
//         fell = true;
//         main_acc +=g;
//         for(int a = 0; a <= lastStatic; a++){
//             branch[a].bone.pos.y1+=main_acc;
//             branch[a].bone.pos.y2 += main_acc;
//         }

//         deltay = main_acc;
//         deltax = 0;
//     }
//     else if(fell){ //that means it just hit the ground
//         fell = false;
//         main_acc = 0;
//         for(int i = 0; i < branch.size(); i++)
//             branch[i].bone.vel = 0;
//         hitGround = branch[0].bone.pos.y1;
//     }
//     if(branch[6].bone.pos.y2 - hitGround> 0 && hitGround != -1){
//         rose = true;
//         deltay = hitGround - branch[6].bone.pos.y2;
//         deltax = 0;
//         for(int a = 0; a <= lastStatic; a++){
//             branch[a].bone.pos.y1 += deltay;
//             branch[a].bone.pos.y2 += deltay ;
//         }
//         if(hitGround >= branch[6].bone.pos.y2){
//             hitGround = -1;
//             rose = false;
//         }
//     }

//     for(int i = branch.size()-1; i> lastStatic; i--){
//         branch_struct *br = &branch[i].bone;
    
//         //beta yra dabartinis kampas, o w prideda/sumazina, kad suktusi
//         double beta = asinf(std::abs(br->pos.y1-br->pos.y2)/br->length);
//         //Proper angles in a full circle
//         if(i != 2){
//             if(inCirclePos(I, &br->pos)) beta = beta;
//             else if(inCirclePos(II, &br->pos)) beta = h_pi - beta + h_pi;
//             else if(inCirclePos(III, &br->pos)) beta = beta + pi;
//             else if(inCirclePos(IV, &br->pos)) beta = h_pi - beta + pi3_2;
//             //3,4 yra rankos. 
//             if((i == 3 || i == 4) && beta >= 0 && beta <= h_pi) beta += 2*pi;            
//         }
//         double acc = deltay;
//         if(main_acc != 0){
//             //acc = sqrtf(main_acc)*g;
//             if(br->pos.x2 < br->pos.x1) acc *= -1;
//             br->vel+=acc;
//         }
//         else{
//             if(std::abs(beta - branch[i].default_angle) > offset){ //return to org pos
//                 acc = g;
//                 if(beta > branch[i].default_angle) acc *= -1;
//                 br->vel += acc;
//             }
//             else{
//                 br->vel/=2;
//             }
//             // else{ //slight tilting --fix
//             //     acc = branch[i].delta_angle/10;
//             //     if(br->pos.x2 < br->pos.x1) acc *= -1;
//             //     if(beta >= branch[i].default_angle + branch[i].delta_angle/2) acc*=-1;
//             //     br->vel += acc;
//             //     //max tilt velocity
//             //     if(std::abs(br->vel) > branch[i].delta_angle/5) br->vel = (branch[i].delta_angle/5)*br->vel / std::abs(br->vel);
//             // }
//         }

//         //slow down based on friction
//         br->vel/=1 + airfriction;

//         //max velocity
//         if(br->vel < -5) br->vel = -5;
//         else if(br->vel > 5) br->vel = 5;

//         double w = br->vel/br->length; //in rads!
//         beta +=w;
//         //Angle logic - restrictions
//         if(beta <= branch[i].minAngle) beta = branch[i].minAngle;
//         else if(beta >= branch[i].maxAngle-0.005) beta = branch[i].maxAngle-0.005;

//         //Update positions
//         br->pos.x2 = br->pos.x1 + cosf(beta) * br->length;
//         br->pos.y2 = br->pos.y1 - sinf(beta) * br->length;

//         //Sync connection 
//         if(fell || rose){ // a skeleton and the body moved (not just angles)
//             branch[i].bone.pos.y1+=deltay;
//             branch[i].bone.pos.y2+=deltay;

//             branch[i].bone.pos.x1+=deltax;
//             branch[i].bone.pos.x2+=deltax;
//         }
//     }
//     return;
// }

double difX, difY;
double acc;
bool fell = false;
bool dragging = false;

void simulation(){
    difX = lastPos.x - branch[0].bone.pos.x1;
    difY = lastPos.y - branch[0].bone.pos.y1;

    //Falling
    if(isFalling() && !dragging){
        acc += g;
        difY+=acc;
        fell = true;
    }
    else if(fell){
        acc = 0;
        fell = false;
        difY = 0;
    }

    //Gauti traukos kampa
    double alfa;    
    if(difX!=0) alfa = atanf(std::abs(difY/difX));
    else alfa = pi3_2;
    position_var temp; temp.x1 = lastPos.x; temp.x2 = branch[0].bone.pos.x1; temp.y1 = lastPos.y; temp.y2 = branch[0].bone.pos.y1;
    if(inCirclePos(I, &temp)) alfa = alfa;
    else if(inCirclePos(II, &temp)) alfa = h_pi - alfa + h_pi;
    else if(inCirclePos(III, &temp)) alfa = alfa + pi;
    else if(inCirclePos(IV, &temp)) alfa = h_pi - alfa + pi3_2;
    //Bet judeti tures y priesinga puse so:
    alfa += pi; if(alfa > 2*pi) alfa-=2*pi;    

    lastPos.x = branch[0].bone.pos.x1;
    lastPos.y = branch[0].bone.pos.y1;

    for(int i = branch.size()-1; i>lastStatic; i--){
        joints *br = &branch[i];

        //Get current angle
        double beta = asinf(std::abs(br->bone.pos.y1-br->bone.pos.y2)/br->bone.length);
        if(inCirclePos(I, &br->bone.pos)) beta = beta;
        else if(inCirclePos(II,&br->bone.pos)) beta = h_pi - beta + h_pi;
        else if(inCirclePos(III, &br->bone.pos)) beta = beta + pi;
        else if(inCirclePos(IV, &br->bone.pos)) beta = h_pi - beta + pi3_2;
            //3,4 yra rankos. 
        if((i == 3 || i == 4) && beta >= 0 && beta <= h_pi) beta += 2*pi;

        double w = (beta - alfa)/100; //100 is the speed koeficient of some sort; beta - alfa ensures that the angle is appropriate, but speeds vary
        
        if(2*pi - beta + alfa < beta - alfa) beta +=w;
        else beta -=w;

        //Restrictions
        if(beta <= br->minAngle) beta = br->minAngle;
        else if(beta >= br->maxAngle-0.005) beta = br->maxAngle-0.005;

        //Update positions
        br->bone.pos.x2 = br->bone.pos.x1 + cosf(beta) * br->bone.length;
        br->bone.pos.y2 = br->bone.pos.y1 - sinf(beta) * br->bone.length;

        br->bone.pos.x1 += difX;
        br->bone.pos.x2 += difX;
        br->bone.pos.y1 += difY;
        br->bone.pos.y2 += difY;
    }
    branch[0].bone.pos.x1 += difX; branch[1].bone.pos.x1 += difX;
    branch[0].bone.pos.x2 += difX; branch[1].bone.pos.x2 += difX;
    branch[0].bone.pos.y1 += difY; branch[1].bone.pos.y1 += difY;
    branch[0].bone.pos.y2 += difY; branch[1].bone.pos.y2 += difY;

    return;
}

void createStructure(int index){
    BASE.x = WIDTH/2; BASE.y = HEIGHT/2;
    //Note that x1 and y1 is the JOINT point (the point at which the whole stick will rotate around)

    if(index == STICKMANAS){
        branch.resize(7);
        lastStatic = 1;
        //Apatine stuburo dalis
            //Points
            branch[0].bone.pos.x1 = BASE.x; branch[0].bone.pos.y1 = BASE.y; branch[0].bone.pos.x2 = BASE.x; branch[0].bone.pos.y2 = BASE.y - branch[0].bone.length;
        //Virsutine stuburo dalis
            //Points    
            branch[1].bone.pos.x1 = BASE.x; branch[1].bone.pos.x2 = BASE.x; branch[1].bone.pos.y1 = branch[0].bone.pos.y2; branch[1].bone.pos.y2 = branch[1].bone.pos.y1 - branch[1].bone.length;
        //Galva
            branch[2].bone.length/=2;
            //Points
            branch[2].bone.pos.x1 = BASE.x; branch[2].bone.pos.x2 = BASE.x; branch[2].bone.pos.y1 = branch[1].bone.pos.y2; branch[2].bone.pos.y2 = branch[2].bone.pos.y1 - branch[2].bone.length;
            //Angles
            branch[2].maxAngle = 120; branch[2].minAngle = 60; branch[2].default_angle = 90; branch[2].delta_angle = 5;
        //Desine ranka
            //Angles
            branch[3].maxAngle = 420; branch[3].minAngle = 120; branch[3].default_angle = 300; branch[3].delta_angle = 10;
            //Points
            branch[3].bone.pos.x1 = BASE.x; branch[3].bone.pos.x2 = branch[3].bone.pos.x1 - sinf(branch[3].default_angle * pi/180) * branch[3].bone.length; branch[3].bone.pos.y1 = branch[1].bone.pos.y2; branch[3].bone.pos.y2 = branch[3].bone.pos.y1 + cosf(branch[3].default_angle*pi/180) * branch[3].bone.length;
        //Kaire ranka
            //Angles
            branch[4].maxAngle = 420; branch[4].minAngle = 120; branch[4].default_angle = 240; branch[4].delta_angle = 10;
            //Points
            branch[4].bone.pos.x1 = BASE.x; branch[4].bone.pos.x2 = branch[4].bone.pos.x1 + sinf(branch[4].default_angle * pi/180) * branch[4].bone.length; branch[4].bone.pos.y1 = branch[1].bone.pos.y2; branch[4].bone.pos.y2 = branch[4].bone.pos.y1 - cosf(branch[4].default_angle*pi/180) * branch[4].bone.length;
        //Desine koja
            branch[5].bone.length*=2;
            //Angles
            branch[5].maxAngle = 360; branch[5].minAngle = 180; branch[5].default_angle = 300; branch[5].delta_angle = 2;
            //Points
            branch[5].bone.pos.x1 = BASE.x; branch[5].bone.pos.x2 = branch[5].bone.pos.x1 + cosf(branch[5].default_angle*pi/180) * branch[5].bone.length; branch[5].bone.pos.y1 = branch[0].bone.pos.y1; branch[5].bone.pos.y2 = branch[5].bone.pos.y1 - sinf(branch[5].default_angle * pi/180) * branch[5].bone.length; 
        //Kaire koja
            branch[6].bone.length*=2;
            //Angles
            branch[6].maxAngle = 360; branch[6].minAngle = 180; branch[6].default_angle = 240; branch[6].delta_angle = 2;
            //Points
            branch[6].bone.pos.x1 = BASE.x; branch[6].bone.pos.x2 = branch[6].bone.pos.x1 + cosf(branch[6].default_angle*pi/180) * branch[6].bone.length; branch[6].bone.pos.y1 = branch[0].bone.pos.y1; branch[6].bone.pos.y2 = branch[6].bone.pos.y1 - sinf(branch[5].default_angle * pi/180) * branch[6].bone.length; 

        //All angles above are in degrees, change to rads;
        for(int i = lastStatic + 1; i < branch.size(); i++){
            branch[i].default_angle *= (pi / 180);
            branch[i].delta_angle *= (pi / 180);
            branch[i].minAngle *= (pi / 180);
            branch[i].maxAngle *= (pi / 180);
        }
        lastPos.x = branch[0].bone.pos.x1;
        lastPos.y = branch[0].bone.pos.y1;
    }
    return;
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Ragdoll", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIDTH, HEIGHT, 0);
    SDL_Renderer* rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event windowEvent;

    createStructure(STICKMANAS);
    
    //Create an object
    objects.resize(1);
    for(int i = 0; i < WIDTH; i++){
        pixel_info temp;
        //temp.state = 1; ar reik
        temp.coord.x = i;
        temp.coord.y = HEIGHT-1;
        objects[0].push_back(temp);
    }

    while(true){
        if(SDL_PollEvent(&windowEvent)){
            if(SDL_QUIT == windowEvent.type || windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_q)
            break;
        }

        simulation();
        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 255,255,255,255);
        drawLines(&rend);

        SDL_RenderPresent(rend);
        SDL_Delay((1/float(UPS))*1000);
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}