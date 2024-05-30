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
    double minAngle;
    double maxAngle;
    //kampas TARP kaulų, kurį stengiasi palaikyti; RADS
    double defaultAngle;
    /**
     * @param dideja tai kampas dideja; 1
     * @param mazeja tai kampas mazeja; -1
    */
    int rotDir;
    //kaulo ilgis
    double length;
    //taškas, nuo kurio "ištiesiamas" kaulas; Matematinės funkcijos visada palaiko juos sujungtus.
    double x;
    double y;
};
enum jointNames{
    K_AUSIS = 0,
    D_AUSIS = 1,
    GALVA = 2,
    K_BLAUZDA = 3,
    D_BLAUZDA = 4,
    K_SLAUNIS = 5,
    D_SLAUNIS = 6,
    KUNELIS = 7,
    K_RANKA = 8,
    D_RANKA = 9,
    
    JOINT_COUNT = 10
};
int CURRENTCATEGORY, CURRENTSPRITENUMB;

//Sprites
int totalSpriteCategoryNumb = 2;
enum class category_test{
    TEST = 0,
    SPRITE_NUMB = 2,
    
    koja = 0, kunelis = 1
};
int pasuktas_order[7] = {3, 5,4,6, 0, 2, 1};
enum class category_pasuktas{
    PASUKTAS = 1,
    SPRITE_NUMB = 8,
    
    ausis_gale = 0, ausis_priekyje = 1, galva = 2, koja_gale = 3, koja_priekyje = 4, kunelis = 5, ranka_priekyje = 6
};

//Išsaugoma 'SDL_Rect' pozicija ir 'SDL_Texture' sprite informacija kiekvienai nuotraukai. 
struct spriteInfo{
    SDL_Surface* img_data;
    SDL_Texture* img;
    //LENGTH YRA KIEK KARTU MAZINTI**
    int length;
    int joint_index;
};
int destinationME = 5;
double g = 0.05;
/*sprites[kategorijos indeksas][kategorijoje esancios nuotraukos indeksas]*/
std::vector<std::vector<spriteInfo>> sprites;
std::vector<JOINT_STRUCT> joints;
std::vector<SDL_Rect> spritePositions;

struct {
    // @param value is the happiness level ranging from 0 to 100; 0 is sad, 30 is neutral, 50 is happy and 70 cheerful  (cheerful > happy)
    double value = 30;
    // @param last tai kada paskutinį kartą buvo pakeistas "value";
    std::chrono::steady_clock::time_point last;
    // @param prassed tai kada buvo paspausta ant petto;
    std::chrono::steady_clock::time_point pressed;

}emotions;

enum time_units{
    microseconds = 0,
    milliseconds = 1,
    seconds = 2,
    minutes = 3,
};

std::string collisions();
void updateBones();
std::chrono::steady_clock::time_point currentTime();
double timeDiff(time_units unit, std::chrono::steady_clock::time_point last_time, std::chrono::steady_clock::time_point current_time);

/**
 * Called by 'createCharacterBones()'
  * @param indeksas tai joint'o numeris, naudoti geriau enum reikšmę.
  * @param baseAngle tai pradinis kampas, bei kampas į kurį kaulai atsistatys.
  * @param minAngle tai MAŽIAUSIAS pasisukimo kampas.
  * @param maxAngle tai DIDŽIAUSIAS pasisukimo kampas.
  * @param length tai kaulo ilgis.
  * @param rot_direction tai nusako į kurią pusę turės kaulai pradėti judėti, kai prasideda judėjimas. '1' kampas didėja, o '-1' mažėja.
*/
void actuallyCreateCharacterBones(int indeksas, double baseAngle, double minAngle, double maxAngle, kaulu_ilgiai length, int rot_direction){
    joints[indeksas] = {baseAngle, minAngle, maxAngle, baseAngle, rot_direction, (double)length, 0, 0};
    return;
}

/**
 * Funckija, sukurianti visus kaulus.
*/
void createCharacterBones(){
    joints.resize(JOINT_COUNT);
    //Kurie neturi judėti visiškai
    joints[KUNELIS] = {pi_2, 0, 0, 0, 0, 0, (double)charStartingPos.x, (double)charStartingPos.y};
    joints[GALVA] = {pi_2, 0, 0, 0, 0, 0, (double)charStartingPos.x, (double)charStartingPos.y - kuno_ilg};

    actuallyCreateCharacterBones(K_AUSIS, rad(88), rad(88), rad(180-88), ausies_ilg, dideja);
    actuallyCreateCharacterBones(D_AUSIS, rad(92), rad(88), rad(180-88), ausies_ilg, mazeja); 
    actuallyCreateCharacterBones(K_SLAUNIS, rad(88), rad(83), rad(180-83), slaunies_ilg, dideja);
    actuallyCreateCharacterBones(D_SLAUNIS, rad(92), rad(83), rad(180-83), slaunies_ilg, mazeja);
    actuallyCreateCharacterBones(K_BLAUZDA, rad(88), rad(90), rad(180-60), blauzdos_ilg, dideja);
    actuallyCreateCharacterBones(D_BLAUZDA, rad(92), rad(90), rad(180-60), blauzdos_ilg, mazeja);
    actuallyCreateCharacterBones(K_RANKA, rad(90), rad(83), rad(180-83), rankos_ilg, dideja);
    actuallyCreateCharacterBones(D_RANKA, rad(90), rad(83), rad(180-83), rankos_ilg, mazeja);

    updateBones();
    return;
}

/**
 * Called by 'updateBones()'
 * @param connect tai 'kažkuris' joint, kurį sujungsime su 'kitu' joint'u.
 * @param to tai tas 'kitas' joint su kuriuo bus sujungtas 'connect'.
*/
void connectbones(jointNames connect, jointNames to){
    joints[connect].x = cosf(joints[connect].ANGLE)*joints[connect].length + joints[to].x;
    joints[connect].y = sinf(joints[connect].ANGLE)*joints[connect].length + joints[to].y;
    return;
}
void updateBones(){
    connectbones(GALVA, KUNELIS);
    connectbones(K_AUSIS, GALVA);
    connectbones(D_AUSIS, GALVA);
    connectbones(K_RANKA, KUNELIS);
    connectbones(D_RANKA, KUNELIS);
    connectbones(K_SLAUNIS, KUNELIS);
    connectbones(D_SLAUNIS, KUNELIS);
    connectbones(K_BLAUZDA, K_SLAUNIS);
    connectbones(D_BLAUZDA, D_SLAUNIS);
    return;
}

/**
 * Simulates basic physical interactions.
*/
void physics(){
    std::string collision_tag = collisions();
    if(collision_tag != "-") velY = 0;
    else velY +=g;
    return;
}

/**
 * Nustato, kada įvyksta 'atsitrenkimas' ('collision').
 * @return "KOJOS", kai veikėjas paliečia platformą. "-", kai veikėjas nieko nepaliečia.
*/
std::string collisions(){
    if(joints[KUNELIS].y + ground + 2 >= HEIGHT - 1){
        joints[KUNELIS].y = HEIGHT-ground;
        return "KOJOS";
    }

    //patikrinti keleta y tasku depenbding on how fast the thing falls
    if(platformPoints.size() > round(joints[KUNELIS].y + 1) * WIDTH - joints[KUNELIS].x + ground*WIDTH)
        if(platformPoints[round(joints[KUNELIS].y + 1) * WIDTH - joints[KUNELIS].x + ground*WIDTH] == 1){
            return "KOJOS";
        }
    return "-"; //no collision
}

/**
 * @param degrees tai kampas laipsniais
 * @return kampą radianais
*/
double rad(double degrees){
    return degrees*pi/180;
}

//ALLOWED TO CHANGE STATE ISNT WORKING
bool allowedToChangeState = true;
bool begin[7] = {true,true,true,true,true, true, true};
void resetPos(){
    int count= 0;
    //i was desperate, its 4am when im writing this
    int indx[6] = {D_SLAUNIS, K_SLAUNIS, D_BLAUZDA, K_BLAUZDA, D_RANKA, K_RANKA};
    for(int i : indx){
        if(std::abs(joints[i].defaultAngle-joints[i].ANGLE) > rad(1)){
            if(joints[i].ANGLE > joints[i].defaultAngle) joints[i].ANGLE -= rad(1);
            else joints[i].ANGLE += rad(1);
        }
        else{
            joints[i].ANGLE = joints[i].defaultAngle;
            count++;
        }
    }
    for(int i = 0; i < 7; i++) begin[i] = true;
    joints[K_SLAUNIS].rotDir = dideja;
    joints[D_SLAUNIS].rotDir = mazeja;
    joints[K_BLAUZDA].rotDir = dideja;
    joints[D_BLAUZDA].rotDir = mazeja;
    joints[K_RANKA].rotDir = dideja;
    joints[D_RANKA].rotDir = mazeja;
    if(count == 6) allowedToChangeState = true;
    return;
}
void animate(){
    //Moving
    if(velX!=0){
        allowedToChangeState = false;
        double speed = velX;
        int direction = velX/(std::abs(velX));
        int indx[4] = {D_SLAUNIS, K_SLAUNIS, D_RANKA, K_RANKA};
        for(int i : indx){
            if(i == D_RANKA || i== K_RANKA) speed = velX/2;
            else speed = velX/1.5;
            if(joints[i].ANGLE > joints[i].maxAngle ) joints[i].rotDir = -1;
            if(joints[i].ANGLE <= joints[i].minAngle) joints[i].rotDir = 1;
            joints[i].ANGLE += speed*joints[i].rotDir/45 * direction;
        }
        int ind[2] = {D_BLAUZDA, K_BLAUZDA};
        speed = velX/1.5;
        //somewhere multiply by direction;
        for(int i : ind){
            if(joints[i+1].ANGLE < pi_2) begin[i] = true;
            if(joints[i].ANGLE > joints[i].maxAngle){
                begin[i] = false;
                joints[i].ANGLE -= rad(speed);
                joints[i].rotDir = -1;
            }
            if(joints[i].ANGLE <= joints[i].minAngle) joints[i].rotDir = 1;
            if(begin[i] && joints[i].rotDir == -1 || joints[i].rotDir == 1)  joints[i].ANGLE += speed*joints[i].rotDir/30;
        }
    }
    //Idle
    if(velX == 0 && velY == 0){
        resetPos();
    }
    return;
}

void prepASprite(const char* path, int category, int obj, SDL_Renderer* renderer, int jointindex, int mazinti){
    SDL_Surface* tmpsurfc;
    tmpsurfc = SDL_LoadBMP(path);

    sprites[category][obj].img_data = new SDL_Surface(*tmpsurfc);
    sprites[category][obj].img = SDL_CreateTextureFromSurface(renderer, tmpsurfc);

    sprites[category][obj].joint_index = jointindex;
    sprites[category][obj].length = mazinti;

    SDL_FreeSurface(tmpsurfc);
    return;
}

void prepareSprites(){
    SDL_Surface* tmpSurfc{};
    spritePositions.resize(JOINT_COUNT);
    sprites.resize(totalSpriteCategoryNumb);
    sprites[(int)category_pasuktas::PASUKTAS].resize((int)category_pasuktas::SPRITE_NUMB);

    prepASprite("./assets/images/pasuktas/ausis_gale.bmp", (int)category_pasuktas::PASUKTAS, (int)category_pasuktas::ausis_gale, rend, K_AUSIS, (int)sumazinti::ausis);
    prepASprite("./assets/images/pasuktas/ausis_priekyje.bmp", (int)category_pasuktas::PASUKTAS, (int)category_pasuktas::ausis_priekyje, rend, D_AUSIS, (int)sumazinti::ausis);
    prepASprite("./assets/images/pasuktas/neutralus_galva.bmp", (int)category_pasuktas::PASUKTAS, (int)category_pasuktas::galva, rend, -1, (int)sumazinti::galva);
    prepASprite("./assets/images/pasuktas/koja_gale.bmp", (int)category_pasuktas::PASUKTAS, (int)category_pasuktas::koja_gale, rend, K_SLAUNIS, (int)sumazinti::kojas);
    prepASprite("./assets/images/pasuktas/koja_priekyje.bmp", (int)category_pasuktas::PASUKTAS, (int)category_pasuktas::koja_priekyje, rend, D_SLAUNIS, (int)sumazinti::kojas);
    prepASprite("./assets/images/pasuktas/kunelis.bmp", (int)category_pasuktas::PASUKTAS, (int)category_pasuktas::kunelis, rend, KUNELIS, (int)sumazinti::kuneli);
    prepASprite("./assets/images/pasuktas/ranka_priekyje.bmp", (int)category_pasuktas::PASUKTAS, (int)category_pasuktas::ranka_priekyje, rend, K_RANKA, (int)sumazinti::rankas);

    return;
}

/**
 * Ekrane parodo paruoštą nuotrauką.
 * @param sprite_info tai nuotraukos informacijos struct'as. Jį duoti funkcijai iš vektoriaus: sprites[kategorijos indeksas][nuotraukos indeksas]
 * @param to_flip nusako, ar nuotrauka turi būti apversta. 0 - ne, 1 - horizontaliai apversti.
*/
void renderSprite(spriteInfo& sprite_info, int to_flip, int i){
    int x = sprite_info.joint_index;
    if(sprite_info.joint_index == -1){
        if(i == (int)category_pasuktas::galva){
            x = GALVA;
            spritePositions[GALVA] = {(int)joints[KUNELIS].x - sprite_info.img_data->w/(2*sprite_info.length), (int)joints[KUNELIS].y - galvos_ilg, sprite_info.img_data->w/sprite_info.length, sprite_info.img_data->h/sprite_info.length};
        }
    }
    else
        spritePositions[sprite_info.joint_index] = {(int)joints[sprite_info.joint_index].x - sprite_info.img_data->w/(2*sprite_info.length), (int)joints[sprite_info.joint_index].y, sprite_info.img_data->w/sprite_info.length, sprite_info.img_data->h/sprite_info.length};
    
    SDL_RenderCopyEx(rend, sprite_info.img, NULL, &spritePositions[x], joints[x].ANGLE*180/pi - 90, new SDL_Point{sprite_info.img_data->w/(2*sprite_info.length), 0}, (SDL_RendererFlip)to_flip);
    return;
}

void applySprites(){
    int flip;
    if(velX != 0 || velX == 0){
        CURRENTCATEGORY = (int)category_pasuktas::PASUKTAS;
        CURRENTSPRITENUMB = (int)category_pasuktas::SPRITE_NUMB;
    }
    //else currentactegory = stovintis;

    if(velX >= 0) flip = 0; //dont flip
    if(velX < 0) flip = 1;  //do flip

    for(int i : pasuktas_order)
        renderSprite(sprites[CURRENTCATEGORY][i], flip, i);

    return;
}

/**
 * Pathfinding algoritmas.
 * WORK to be IN PROGRESS 
*/
void pathfinding(){
    while(togoPoints.size() > 0){
        if(std::abs(togoPoints[0].x-joints[KUNELIS].x) < destinationME)
            togoPoints.erase(togoPoints.begin());
        else break;
    }

    if(togoPoints.size()>0){
        if(togoPoints[0].x > joints[KUNELIS].x) velX = 1;
        else velX = -1;
    }
    else velX = 0;
    return;
}

/**
 * Viena funkcija, kuri rodys visus kaulus ir taškus ir kelią, kurį kiekvienas objektas turės eiti.
 * Su laiku papildyti.
*/
// void debuglines(){
//     SDL_SetRenderDrawColor(rend, 0,255,0,255);
//     if(togoPoints.size()>0) SDL_RenderDrawLine(rend, joints[KUNELIS].x, joints[DUBUO].y, togoPoints[0].x, joints[DUBUO].y);

//     SDL_SetRenderDrawColor(rend, 255,255,255,255);
//     for(JOINT_STRUCT joint : joints)
//         SDL_RenderDrawPoint(rend, joint.x, joint.y);
//     SDL_RenderDrawLine(rend, joints[K_PEDA].x, joints[K_PEDA].y, joints[K_KELIS].x, joints[K_KELIS].y);
//     SDL_RenderDrawLine(rend, joints[D_PEDA].x, joints[D_PEDA].y, joints[D_KELIS].x, joints[D_KELIS].y);

//     SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, joints[K_KELIS].x, joints[K_KELIS].y);
//     SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, joints[D_KELIS].x, joints[D_KELIS].y);
//     return;
// }

/**
 * Tikrina, ką naudotojas daro su veikėju.
*/
bool clicked = false;
std::string isUserInteractingWithCharacter(){
    //Its only once, right?
    //Kol kas sdl nustato paspaudima, tik kai ant pacios programos paspaudziama - kol kas tai tik petto!; Gali glostyti kai net neglosto !  
    if(evt.button.button == SDL_BUTTON_LEFT){
        switch(evt.type){
            case SDL_MOUSEBUTTONDOWN:
                clicked = true;
                emotions.pressed = currentTime();
            break;
            
            case SDL_MOUSEBUTTONUP:
                clicked = false;
                if(timeDiff(milliseconds, emotions.pressed, currentTime()) <= 200)
                return "skriaudzia";    
        }
    }

    if(clicked && timeDiff(milliseconds, emotions.pressed, currentTime()) > 200)
        return "glosto";
    
    return "-";
}

/**
 * function that deals with all emotion and action (do something when something is (not) happening).
*/
void emotionAndActionController(){
    std::string output = isUserInteractingWithCharacter();

    //petto's emotion values are affected
    if(output != "-"){
        if(output == "glosto"){
            emotions.value +=0.01;
        }
        
        if(output == "skriaudzia"){
            emotions.value -=2;
        }
    }
    else if(timeDiff(minutes, emotions.last, currentTime()) >= 10){
        //praejo x min nuo last activity;
        emotions.value -= 1;

        emotions.last = currentTime();
    }

    //Skalės ribos
    emotions.value = std::max(emotions.value, (double)0);
    emotions.value = std::min(emotions.value, (double)100);

    //Petto decides how to act (how to mvoe, which sprite to use, etc)

    return;
}
// @return grąžina dabartini laiką kaip time.point variable.
std::chrono::steady_clock::time_point currentTime(){
    return std::chrono::steady_clock::now();
}

/**
 * Funckija iššreiškianti praėjusį laiką tarp 2 skirtingų datų. Datos turi būti variables 'std::chrono::steady_clock::time_point'.
 * @param unit tai kokiu laiko vienetu gražinti skaičių.
 * @param last_time tai time_point paskutinio išsaugoto laiko.
 * @param current_time tai time_point dabartinio laiko.
 * @return Grąžina praėjusį laiką tarp 2 time_points tam tikru laiko vienetu.
*/
double timeDiff(time_units unit, std::chrono::steady_clock::time_point last_time, std::chrono::steady_clock::time_point current_time){
    switch(unit){
        case microseconds:
            return std::chrono::duration_cast<std::chrono::microseconds>(current_time-last_time).count();
        case milliseconds:
            return std::chrono::duration_cast<std::chrono::milliseconds>(current_time-last_time).count();
        case seconds:
            return std::chrono::duration_cast<std::chrono::seconds>(current_time-last_time).count();
        case minutes:
            return std::chrono::duration_cast<std::chrono::minutes>(current_time-last_time).count();
    }
    std::cout << "WRONG TIME UNIT (incorrect index or function is outdated)\n";
    return -1;
}

/**
 * Calls all functions for the character to work properly.
*/
void processCharacter(){
    emotionAndActionController();

    physics();
    pathfinding();

    animate();

    joints[KUNELIS].x += velX;
    joints[KUNELIS].y += velY;
    updateBones();

    applySprites();

    //debuglines();
    return;
}