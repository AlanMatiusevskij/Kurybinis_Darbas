#include"includes.h"

const int WIDTH = 1500, HEIGHT = 600, UPS = 144;
const double pi = 3.14159265359; 
double h_pi = pi/2, pi3_2 = pi * 3 /2, pi2 = 2*pi;

//Tiesiog structs kad laikyti 2 arba 1 pikselio pozicijas (patogumui)
struct position_var{
    double x1;
    double x2;
    double y1;
    double y2;
};
struct pixel_var{
    double x;
    double y;
    double z;
};
struct velocitystruct{
    double x;
    double y;
    double z;
};
struct JOINTPOINTSTRUCT{
    pixel_var pos;
    //indexes, but if -1, thats ground
    std::vector<int> interactingWith;
    int mass;
    std::vector<pixel_var> desiredDistance; //Distance it keeps between itself and an another point
    velocitystruct velocity;
};
std::vector<JOINTPOINTSTRUCT> point;
int maxPixelEM = 2;
//Laikoma informacija taškų, į kuriuos reikia dabar nueiti.
std::vector<pixel_var> KELIAS;
//pixels per second
double MAXvelocity = 1; 

//Starting position (lower spine);
pixel_var BASE;
//Mouse positions
int My1, My2, Mx1, Mx2;

std::vector<position_var> floors;

enum circle_parts{
    I = 1,
    II = 2,
    III = 3,
    IV = 4
};
enum structure_names{
    STICKMANAS = 3
};

//PASAKO, kuriame apskritimo dalyje taškas yra
bool inCirclePos(int index, position_var *pos){
    if(index == I){
        if(pos->x2 > pos->x1 && pos->y2 <= pos->y1)  return true;
    }
    else if(index == II){
        if(pos->x2 < pos->x1 && pos->y2 <= pos->y1)  return true; 
    }
    else if(index == III){
        if(pos->x2 <= pos->x1 && pos->y2 > pos->y1)  return true;
    }
    else if(index == IV){
        if(pos->x2 >= pos->x1 && pos->y2 > pos->y1)  return true;
    }

    return false;
}

double squared(double value){
    return value*value;
}

float g = 0.001; 
void jointSim(){
    for(JOINTPOINTSTRUCT &joint : point){
        for(int i = 0; i < joint.interactingWith.size(); i++){
            if(joint.interactingWith[i] == -1){ //interacting with ground; just -g for now
                if(std::abs(joint.pos.y - HEIGHT*8/10) > maxPixelEM) joint.velocity.y += g*joint.mass;
                else joint.velocity.y = 0;
                joint.pos.y += joint.velocity.y;
                joint.velocity.y /=1.001;
            }
            else{   
                joint.pos.y+= (joint.desiredDistance[i].y-joint.pos.y+point[joint.interactingWith[i]].pos.y)/10;
                joint.pos.x+= (joint.desiredDistance[i].x-joint.pos.x+point[joint.interactingWith[i]].pos.x)/10;
            }
        }
    }


    return;
}

void CHARCONTROL(){
    //Simulate all points
    jointSim();

    //Jeigu taip, tai reikia eiti
    if(KELIAS.size() != 0){
        
    }
    //Idling
    else{

    }

    return;
}

void ANIMATIONCONTROL(){

    return;
}
int raiseKELISRIGHTX = 0, raiseKELISRIGHTY = -40;
int raiseRIGTHFOOTX = 0, raiseRIGHTFOOTY = -20;

int restKELISRIGHTX = 0, restKELISRIGHTY = -20;
int restRIGTHFOOTX = 0, restRIGHTFOOTY = 0;

Uint32 timer(Uint32 interval, void* nothing){
    std::cout << "check";
    if(point[1].desiredDistance[1].y == 0){//raise
        point[1].desiredDistance[0].y = raiseRIGHTFOOTY;
        point[3].desiredDistance[0].y = raiseKELISRIGHTY;
    }
    else{//lower
        point[1].desiredDistance[0].y = restRIGHTFOOTY;
        point[3].desiredDistance[0].y = restKELISRIGHTY;
    }
    return interval;
}

void createStructure(int index){
    BASE.x = WIDTH/2; BASE.y = HEIGHT/2;
    if(index == STICKMANAS){
        point.resize(5);
        //kairiosio pėdos taškas
        point[0] = {{BASE.x, BASE.y, 0}, {-1, 2}, 7, {{0, 0, 0}, {0, 20, 0}}, {0,0,0}};
        //dešiniosios pėdos taškas
        point[1] = {{BASE.x+15, BASE.y, 0}, {-1, 3}, 7, {{0,0,0}, {0,20,0}}, {0,0,0}};
        //Kairysis kelis
        point[2] = {{BASE.x+5, BASE.y-20, 0}, {0}, 6, {{0, -20, 0}}, {0,0,0}};
        //Desinysis kelis
        point[3] = {{BASE.x+50, BASE.y-20, 0}, {1}, 6, {{0, -20, 0}}, {0,0,0}};
        // //Dubuo
        point[4] = {{BASE.x+20, BASE.y-40, 0}, {2, 3}, 5, {{4, -50, 0}, {-4, -50, 0}}, {0,0,0}};
        // //Kairysis petys
        // point[5] = {{BASE.x+45, BASE.y-150, 0}, {4, 7, 9, 0, 1}, 4, {100, 50, 10, 100}, {0,0,0}};
        // //Desinysis petys
        // point[6] = {{BASE.x-15, BASE.y-150, 10}, {4, 8, 9, 0, 1}, 4, {100, 50, 10, 100}, {0,0,0}};
        // //Kairysis delnas
        // point[7] = {{BASE.x-10, BASE.y-100, -10}, {5}, 3, {50}, {0,0,0}};
        // //Desinysis delnas
        // point[8] = {{BASE.x+40, BASE.y-100, 0}, {6}, 3, {50}, {0,0,0}};
        // //Kaklas
        // point[9] = {{BASE.x, BASE.y-200, 0}, {5, 6, 10, 0, 1}, 2, {10,10, 60, 200, 200}, {0,0,0}};
        // //Virsugalvis
        // point[10] = {{BASE.x, BASE.y-250, 0}, {9, 0, 1}, 1, {60, 210, 210}, {0,0,0}};
    }

    return;
}

//Various debugging stuff (make it more complex)
void DEBUGGING(SDL_Renderer *rend){
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderDrawLine(rend, point[0].pos.x, point[0].pos.y, point[2].pos.x, point[2].pos.y);
    SDL_RenderDrawLine(rend, point[1].pos.x, point[1].pos.y, point[3].pos.x, point[3].pos.y);
    SDL_RenderDrawLine(rend, point[2].pos.x, point[2].pos.y, point[4].pos.x, point[4].pos.y);
    SDL_RenderDrawLine(rend, point[3].pos.x, point[3].pos.y, point[4].pos.x, point[4].pos.y);
    // SDL_RenderDrawLine(rend, point[4].pos.x, point[4].pos.y, point[5].pos.x, point[5].pos.y);
    // SDL_RenderDrawLine(rend, point[4].pos.x, point[4].pos.y, point[6].pos.x, point[6].pos.y);
    // SDL_RenderDrawLine(rend, point[5].pos.x, point[5].pos.y, point[7].pos.x, point[7].pos.y);
    // SDL_RenderDrawLine(rend, point[6].pos.x, point[6].pos.y, point[8].pos.x, point[8].pos.y);
    // SDL_RenderDrawLine(rend, point[5].pos.x, point[5].pos.y, point[9].pos.x, point[9].pos.y);
    // SDL_RenderDrawLine(rend, point[6].pos.x, point[6].pos.y, point[9].pos.x, point[9].pos.y);
    // SDL_RenderDrawLine(rend, point[9].pos.x, point[9].pos.y, point[10].pos.x, point[10].pos.y);

    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    for(JOINTPOINTSTRUCT joint : point){
        //make circles
        SDL_RenderDrawPoint(rend, joint.pos.x, joint.pos.y);
        SDL_RenderDrawPoint(rend, joint.pos.x, joint.pos.y+1);
        SDL_RenderDrawPoint(rend, joint.pos.x, joint.pos.y-1);
        SDL_RenderDrawPoint(rend, joint.pos.x+1, joint.pos.y);
        SDL_RenderDrawPoint(rend, joint.pos.x-1, joint.pos.y);
    }
    return;
}
SDL_TimerID timr;
int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow("ragdoll.cpp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIDTH, HEIGHT, 0);
    SDL_Renderer* rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event windowEvent;

    createStructure(STICKMANAS);

    //Create a temp floor;
    position_var tmp; tmp.x1 = 0; tmp.x2 = WIDTH -1; tmp.y1 = HEIGHT*8/10; tmp.y2 = HEIGHT*8/10;
    floors.push_back(tmp);
    
    while(true){
        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);
        if(SDL_PollEvent(&windowEvent)){
            if(SDL_QUIT == windowEvent.type || windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_q)
                break;
        int nth;
            if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_r)
                timr = SDL_AddTimer(500, timer, (void*)nth);
            if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_t)
                SDL_RemoveTimer(timr);
        }

        CHARCONTROL();

        if(windowEvent.type == SDL_MOUSEBUTTONDOWN && windowEvent.button.button == SDL_BUTTON_LEFT)
            SDL_GetMouseState(&Mx2, &My2);

        if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_k){
            point[0].pos.y-=50;
            point[1].pos.y-=50;
        }

        //simulation(); - kinda now a falling/dragging simulation
        SDL_SetRenderDrawColor(rend, 255,255,255,255);
        SDL_RenderDrawLine(rend, floors[0].x1, floors[0].y1, floors[0].x2, floors[0].y2);

        DEBUGGING(rend);

        SDL_RenderPresent(rend);
        SDL_Delay((1/float(UPS))*1000);
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}
//note: 0 AI or any other teaching/copying material (except SDL/c++ wiki and stack overflow when needed) involved ;)

    // std::cout << ("\033[31mThis is red font.\033[0m") << "\n";
    // std::cout << ("\033[32mThis is green font.\033[0m")<< "\n";
    // std::cout << ("\033[33mThis is yellow font.\033[0m")<< "\n";
    // std::cout << ("\033[34mThis is orange font.\033[0m THIS IS WHITE")<< "\n";
    //37 is white