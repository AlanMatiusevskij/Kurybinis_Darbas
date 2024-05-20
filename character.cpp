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
    K_PEDA = 0,
    K_KELIS = 1,
    D_PEDA = 2,
    D_KELIS = 3,
    DUBUO = 4,
    JOINT_COUNT = 5
};
enum spriteIndexes{
  //category_*kategorijosPavadinimas*, category*KategorijosPavadinimas*_SpriteNumb, *kategorija*_*nuotraukosPavadinimas*. (nuotrauka=sprite)
    category_test = 0, categoryTest_SpriteNumb = 2, test_koja = 0, test_kunelis = 1,

    //Kiek iš viso kategorijų.
    totalSpriteCategoryNumb = 1
};
//Išsaugoma 'SDL_Rect' pozicija ir 'SDL_Texture' sprite informacija kiekvienai nuotraukai. 
struct spriteInfo{
    SDL_Surface* img_data;
    SDL_Texture* img;
};
int feet_index[2] = {K_PEDA, D_PEDA};
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
    joints[DUBUO] = {pi_2, 0, 0, 0, 0, 0, (double)charStartingPos.x, (double)charStartingPos.y};

    actuallyCreateCharacterBones(K_KELIS, rad(88), rad(78), rad(180-78), thigh, dideja);
    actuallyCreateCharacterBones(D_KELIS, rad(92), rad(78), rad(180-78), thigh, mazeja);
    actuallyCreateCharacterBones(K_PEDA, rad(88), rad(90), rad(180-60), calf, dideja);
    actuallyCreateCharacterBones(D_PEDA, rad(92), rad(90), rad(180-60), calf, mazeja);

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
    connectbones(K_KELIS, DUBUO);
    connectbones(D_KELIS, DUBUO);
    connectbones(K_PEDA, K_KELIS);
    connectbones(D_PEDA, D_KELIS);
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
    if(joints[DUBUO].y + ground + 2 >= HEIGHT - 1){
        joints[DUBUO].y = HEIGHT-ground;
        return "KOJOS";
    }

    //patikrinti keleta y tasku depenbding on how fast the thing falls
    if(platformPoints.size() > round(joints[DUBUO].y + 1) * WIDTH - joints[DUBUO].x + ground*WIDTH)
        if(platformPoints[round(joints[DUBUO].y + 1) * WIDTH - joints[DUBUO].x + ground*WIDTH] == 1){
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
bool begin[5] = {true,true,true,true,true};
void resetPos(){
    int count= 0;
    //i was desperate, its 4am when im writing this
    int indx[4] = {D_KELIS, K_KELIS, D_PEDA, K_PEDA};
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
    for(int i = 0; i < 5; i++) begin[i] = true;
    joints[K_KELIS].rotDir = dideja;
    joints[D_KELIS].rotDir = mazeja;
    joints[K_PEDA].rotDir = dideja;
    joints[D_PEDA].rotDir = mazeja;
    if(count == 4) allowedToChangeState = true;
    return;
}

void animate(){
    //Moving
    if(velX!=0){
        allowedToChangeState = false;
        double speed = velX;
        int direction = velX/(std::abs(velX));
        int indx[2] = {D_KELIS, K_KELIS};
        for(int i : indx){
            if(joints[i].ANGLE > joints[i].maxAngle ) joints[i].rotDir = -1;
            if(joints[i].ANGLE <= joints[i].minAngle) joints[i].rotDir = 1;
            joints[i].ANGLE += speed*joints[i].rotDir/45 * direction;
        }
        indx[0] = D_PEDA;
        indx[1] = K_PEDA;

        //somewhere multiply by direction;
        for(int i : indx){
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

void prepareSprites(){
    SDL_Surface* tmpSurfc{};
    spritePositions.resize(JOINT_COUNT);
    sprites.resize(totalSpriteCategoryNumb);
    // sprites[category_test].resize(categoryTest_SpriteNumb);

    // tmpSurfc = SDL_LoadBMP("./assets/images/test/kojele.bmp");
    // sprites[category_test][test_koja].img_data = tmpSurfc;
    // sprites[category_test][test_koja].img = SDL_CreateTextureFromSurface(rend, tmpSurfc);

    // SDL_FreeSurface(tmpSurfc);
    // tmpSurfc = SDL_LoadBMP("./assets/images/test/kunelis.bmp");
    // sprites[category_test][test_kunelis].img_data = tmpSurfc;
    // sprites[category_test][test_kunelis].img = SDL_CreateTextureFromSurface(rend, tmpSurfc);
    return;
}

/**
 * Ekrane parodo paruoštą nuotrauką.
 * @param sprite_info tai nuotraukos informacijos struct'as. Jį duoti funkcijai iš vektoriaus: sprites[kategorijos indeksas][nuotraukos indeksas]
 * @param dydis tai kiek kartų nuotrauka turi būti sumažinta. Naudoti enum reikšmes.
 * @param joint_index tai joint'o, prie kurio turi būti 'prilipintas' sprite, pavadinimas.
 * @param to_flip nusako, ar nuotrauka turi būti apversta. 0 - ne, 1 - horizontaliai apversti.
*/
void renderSprite(spriteInfo& sprite_info, sprite_x_kartu_sumazintas_dydis dydis, jointNames joint_index, int to_flip){
    spritePositions[joint_index] = {(int)joints[joint_index].x - sprite_info.img_data->w/(2*dydis), (int)joints[joint_index].y, sprite_info.img_data->w/dydis, sprite_info.img_data->h/dydis};
    SDL_RenderCopyEx(rend, sprite_info.img, NULL, &spritePositions[joint_index], joints[joint_index].ANGLE*180/pi - 90, new SDL_Point{sprite_info.img_data->w/(2*dydis), 0}, (SDL_RendererFlip)to_flip);
    return;
}

void applySprites(){
    int flip;
    if(velX >= 0) flip = 0; //dont flip
    if(velX < 0) flip = 1;  //do flip

    renderSprite(sprites[category_test][test_koja], kojuDydis, K_KELIS, flip);
    renderSprite(sprites[category_test][test_kunelis], kunelioDydis, DUBUO, flip);
    renderSprite(sprites[category_test][test_koja], kojuDydis, D_KELIS, flip);

    return;
}

/**
 * Pathfinding algoritmas.
 * WORK to be IN PROGRESS 
*/
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

/**
 * Viena funkcija, kuri rodys visus kaulus ir taškus ir kelią, kurį kiekvienas objektas turės eiti.
 * Su laiku papildyti.
*/
void debuglines(){
    SDL_SetRenderDrawColor(rend, 0,255,0,255);
    if(togoPoints.size()>0) SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, togoPoints[0].x, joints[DUBUO].y);

    SDL_SetRenderDrawColor(rend, 255,255,255,255);
    for(JOINT_STRUCT joint : joints)
        SDL_RenderDrawPoint(rend, joint.x, joint.y);
    SDL_RenderDrawLine(rend, joints[K_PEDA].x, joints[K_PEDA].y, joints[K_KELIS].x, joints[K_KELIS].y);
    SDL_RenderDrawLine(rend, joints[D_PEDA].x, joints[D_PEDA].y, joints[D_KELIS].x, joints[D_KELIS].y);

    SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, joints[K_KELIS].x, joints[K_KELIS].y);
    SDL_RenderDrawLine(rend, joints[DUBUO].x, joints[DUBUO].y, joints[D_KELIS].x, joints[D_KELIS].y);
    return;
}

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

    joints[DUBUO].x += velX;
    joints[DUBUO].y += velY;
    updateBones();

    applySprites();
    //debuglines();
    return;
}