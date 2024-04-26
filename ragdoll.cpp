#include"includes.h"

const float pi = 3.14159265359;
      float h_pi = pi/2, pi3_2 = pi * 3 / 2;
const int WIDTH = 1500, HEIGHT = 600, UPS = 144;

//Tiesiog structs kad laikyti 2 arba 1 pikselio pozicijas (patogumui)
struct position_var{
    double x1, x2, y1, y2;
};
struct pixel_var{
    double x, y;
};
//Pagrindiniai structs kaulams
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
//All joints
std::vector<joints> branch;
//Starting position (lower spine);
pixel_var BASE;
//Last saved position 
pixel_var lastPos;
//static branches are first ones in the list
int lastStatic = -1;
//Mouse positions
int My1, My2, Mx1, Mx2;
//Variables for angles; EM - Error of Margin
double angle_acc = 0.01, rotationAngle = -1, angleSpeed[7] = {0,0,0,0,0,0,0}, angleEM = 0.01, maxAnglespeed = 0.03;
//Variables for movement
double move_speedX = 0, move_speedY = 0, move_acc = 0.01, maxMove_speed = 1, difX = 0, difY = 0, moveEM = 5;
double Fangle = -1, g = 0.01;

std::vector<position_var> floors;

enum circle_parts{
    I = 1,
    II = 2,
    III = 3,
    IV = 4
};
enum structure_names{
    STICKMANAS = 1,
    PAGALIUKAS = 2 // nebeveikia .-.
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

//PAKEIČIA kampą, kad atitiktų apskritimą
void circleAngle(position_var *pos, double &angle){
    if(inCirclePos(I, pos)) angle = angle;
    else if(inCirclePos(II, pos)) angle = pi - angle;
    else if(inCirclePos(III, pos)) angle += pi;
    else if(inCirclePos(IV, pos)) angle = 2*pi - angle;
    return;  
}

void changePos(double angle, double multiplier){
    //Nustatyti kūno judėjimo kryptį
    move_speedX += cosf(angle)*multiplier;
    move_speedY += sinf(angle)*multiplier;

    if(std::abs(move_speedX) > maxMove_speed) move_speedX = maxMove_speed * (move_speedX/(std::abs(move_speedX)));
    if(std::abs(move_speedY) > maxMove_speed) move_speedY = maxMove_speed * (move_speedY/(std::abs(move_speedY)));
    
    difX += move_speedX;
    difY += move_speedY;

    if(std::abs(difX) < 0.01) difX = 0;

    return;
}
//koju indeksai
int legIndex[2] = {5,6};
void isClipping(){
    for(int i = 0; i < 2; i++){
        joints *leg = &branch[legIndex[i]];
        if(leg->bone.pos.y2 > floors[0].y1 || leg->bone.pos.y1 > floors[0].y1){ //needs a horizontal chekck; a loop thorugh all floors
            move_speedY = 0;
            changePos(leg->default_angle + pi, move_acc);
        }
    }

    return;
}

int legsArray[2]{5,6};
int maxPixelEM = 2;
//note that all objects must and should be only horizontal (y1 == y2)
//note that x1 and y1 positions must and should be closer to 0;0 point
int isFalling(){
    int numb = 0;
    for(int index : legsArray){
        position_var *pos = &branch[index].bone.pos;
        for(position_var object : floors){
            if(std::abs(pos->y2 - object.y2) < maxPixelEM && pos->x2 + maxPixelEM >= object.x1 && pos->x2 - maxPixelEM <= object.x2);
                //all good
            else
                numb++;
        }
    }

    if(numb == sizeof(legsArray)/4) return 0; //FALLING
    if(numb > 0)                    return 1; //atleast one leg dandling
                                    return -1; //not falling
}

struct MOVEMENT_PROCESS{
    std::vector<pixel_var> POINTS; //to reach
    
};

void MOVEMENT(int CONDITION){ //0 PROCEED with tasks from before


    return; 
}

void checkMovementStatus(){
    //3 stages: if no support present - fall vertically;
            //  IF not falling, Move towards a point that is accessible
            //  iff the point is not accessible and the character is not falling, find a way to jump towards the position

    int returnValue = isFalling();//checks all legs;, If alteast one is hovering: moves or jumps away; -1 - not falling; 0, falling; 1- alteast one leg dangling
    if(returnValue == -1) MOVEMENT(0); //carry out what youv been doing before
    if(returnValue == 0); //FALL in place (keep intertia if in the middle of moving)
    if(returnValue == 1); //find where to put legs/move to the side; If not possible, jump off;

    return;
}

//Pagrindiniai skeleto judesiai
void simulation(){
    // changePos(pi3_2, g);
    // isClipping();
    // rotationAngle = Fangle;
    
    checkMovementStatus();

    //Loop through all moving joints
    for(int i = branch.size()-1; i > lastStatic; i--){
        //Pointer to the current joint
        joints *J = &branch[i];

        //Current angle of the pointer J
        double beta = asinf(std::abs(J->bone.pos.y1 - J->bone.pos.y2)/J->bone.length);
        circleAngle(&J->bone.pos, beta);
            //3 ir 4 yra rankos
        if((i == 3 || i == 4) && beta > 0 && beta <= h_pi) beta += 2*pi;

        //The angle of the pulling force (restrained to 2π degrees)
        double w1 = rotationAngle + pi;
        if(w1 > 2*pi) w1 -= 2*pi;

        //Reikia <2π kampo variable, dėl rankų ir jų kampų reguliavimo
        double tBeta = beta;
        if(tBeta > 2*pi) tBeta -= 2*pi;

        //This is scuffed and stupid:
            //Works properly for some cases
        int k = 0;
        if(w1 - tBeta > 0){
            k = 1;
        }
        else k = -1;
            //But fixes (i hope) the rest
        if(rotationAngle < pi && rotationAngle > h_pi && tBeta < rotationAngle) k = -1;
        if(rotationAngle < pi3_2 && rotationAngle > pi && tBeta > rotationAngle) k = 1;
        if(rotationAngle < 2*pi && rotationAngle > pi3_2 && rotationAngle < tBeta) k = 1;
        if(rotationAngle > 0 && rotationAngle < h_pi && rotationAngle > tBeta) k = -1;
            //this doesn't work universally (a single bone does not work for some reason)
        
        //Nustatyt greitį ir jo kryptį
        angleSpeed[i] += k * angle_acc;        
        if(std::abs(angleSpeed[i]) > maxAnglespeed) angleSpeed[i] = maxAnglespeed * (angleSpeed[i]/std::abs(angleSpeed[i]));
        //Pakeisti kampą
        if(Fangle != -1) beta += angleSpeed[i];

        //Angle Constraints:
        // if(beta > J->maxAngle - angleEM) beta = J->maxAngle - angleEM;
        // else if(beta < J->minAngle + angleEM) beta = J->minAngle + angleEM;

        //Update joint angles
        J->bone.pos.x2 = J->bone.pos.x1 + cosf(beta) * J->bone.length;
        J->bone.pos.y2 = J->bone.pos.y1 - sinf(beta) * J->bone.length;

        //Legs move the whole body
        if((i == 5 || i == 6) && Fangle != -1)
            changePos(Fangle, move_acc);
    }
    //Nuo judėjimo priklauso kampų sukimosi greitis; Čia visaip galima išreikšti, kol kas tik suma moduliu;
    angle_acc = std::abs((move_speedX + move_speedY));
    

    //Update positions for all joints - sinchronizuot visus kaulų poziijos pakitimus
    for(int i = 0; i < branch.size(); i++){
        branch[i].bone.pos.x1 += difX;
        branch[i].bone.pos.x2 += difX;
        branch[i].bone.pos.y1 -= difY;
        branch[i].bone.pos.y2 -= difY;
    }

    difX = 0;
    difY = 0;

    return;
}

void createStructure(int index){
    BASE.x = WIDTH/2; BASE.y = HEIGHT/2;
    //Note that x1 and y1 is the JOINT point (the point at which the whole stick will rotate around)

    if(index == STICKMANAS){
        branch.resize(7);
        lastStatic = 1;
        //Apatine stuburo dalis
            //Angles
            branch[0].default_angle = h_pi; branch[0].maxAngle = h_pi; branch[0].minAngle = h_pi; branch[0].delta_angle = 0;
            //Points
            branch[0].bone.pos.x1 = BASE.x; branch[0].bone.pos.y1 = BASE.y; branch[0].bone.pos.x2 = BASE.x; branch[0].bone.pos.y2 = BASE.y - branch[0].bone.length;
        //Virsutine stuburo dalis
            //angles
            branch[1].default_angle = h_pi; branch[1].maxAngle = h_pi; branch[1].minAngle = h_pi; branch[1].delta_angle = 0;
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
    }
    if(index == PAGALIUKAS){
        branch.resize(1);
        lastStatic = -1;
        branch[0].default_angle = 270; branch[0].maxAngle = 420; branch[0].minAngle = 120; branch[0].delta_angle = 10;
        branch[0].bone.pos.x1 = BASE.x; branch[0].bone.pos.x2 = BASE.x; branch[0].bone.pos.y1 = BASE.y; branch[0].bone.pos.y2 = BASE.y + branch[0].bone.length;
    }

        //All angles above are in degrees, change to rads;
    for(int i = lastStatic + 1; i < branch.size(); i++){
        branch[i].default_angle *= (pi / 180);
        branch[i].delta_angle *= (pi / 180);
        branch[i].minAngle *= (pi / 180);
        branch[i].maxAngle *= (pi / 180);
    }
    lastPos.x = BASE.x; lastPos.y = BASE.y;
    return;
}

//Fangle = F(-force)angle; priklauso nuo tasko, į kurį eiti/siekti turėtu kūnas
void applyFangle(){
    Mx1 = branch[0].bone.pos.x1; My1 = branch[0].bone.pos.y1;
    if(Mx2 != Mx1 || My2 != My1){
        if(Mx2-Mx1 == 0){
            if(My2 - My1 > 0)
                Fangle = pi3_2;
            else Fangle = h_pi;
        }
        else{
            Fangle = atanf((double) std::abs(My2-My1) / std::abs(Mx2-Mx1));
            position_var tmp_var; tmp_var.x1 = Mx1; tmp_var.x2 = Mx2; tmp_var.y1 = My1; tmp_var.y2 = My2;
            circleAngle(&tmp_var, Fangle);
        }
    }
    return;
}

//Various debugging stuff (make it more complex)
void DEBUGGING(SDL_Renderer *rend){
    SDL_RenderDrawPoint(rend, Mx2, My2);
    SDL_RenderDrawPoint(rend, Mx2, My2+1);
    SDL_RenderDrawPoint(rend, Mx2, My2-1);
    SDL_RenderDrawPoint(rend, Mx2+1, My2);
    SDL_RenderDrawPoint(rend, Mx2-1, My2);

    SDL_SetRenderDrawColor(rend, 0,255,0,255);
    SDL_RenderDrawLine(rend, branch[0].bone.pos.x1, branch[0].bone.pos.y1, Mx2, My2);
}

//Nupiešti skeletą (patobulint)
void drawLines(SDL_Renderer**rend){
    for(joints item : branch)
        SDL_RenderDrawLine(*rend, roundf(item.bone.pos.x1), roundf(item.bone.pos.y1), roundf(item.bone.pos.x2), roundf(item.bone.pos.y2));

    return;
}

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
        if(SDL_PollEvent(&windowEvent)){
            if(SDL_QUIT == windowEvent.type || windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_q)
            break;
        }

        if(windowEvent.type == SDL_MOUSEBUTTONDOWN && windowEvent.button.button == SDL_BUTTON_LEFT)
            SDL_GetMouseState(&Mx2, &My2);
        //applyFangle();

        simulation();

        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 255,255,255,255);
        drawLines(&rend);

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