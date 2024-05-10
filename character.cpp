#include"includes.h"
/*
    @AlanMatiusevskij
    @Gantzomara
    įdomu ką šitie daro
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
    //y kuria puse suktis
    bool rotDir;
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
//sprites[kategorijos indeksas][kategorijoje esancios nuotraukos indeksas]
std::vector<std::vector<spriteInfo>> sprites;
std::vector<JOINT_STRUCT> joints;
std::vector<SDL_Rect> spritePositions;

std::string collisions();
void updateBones();

/**
 * Called by 'createCharacterBones()'
  * @param indeksas tai joint'o numeris, naudoti geriau enum reikšmę.
  * @param baseAngle tai pradinis kampas, bei kampas į kurį kaulai atsistatys.
  * @param minAngle tai MAŽIAUSIAS pasisukimo kampas.
  * @param maxAngle tai DIDŽIAUSIAS pasisukimo kampas.
  * @param length tai kaulo ilgis.
  * @param rot_direction tai nusako į kurią pusę turės kaulai pradėti judėti, kai prasideda judėjimas. 'true' kampas didėja, o 'false' mažėja.
*/
void actuallyCreateCharacterBones(int indeksas, double baseAngle, double minAngle, double maxAngle, kaulu_ilgiai length, bool rot_direction){
    joints[indeksas] = {baseAngle, minAngle, maxAngle, baseAngle, rot_direction, (double)length, 0, 0};
    return;
}

/**
 * Funckija, sukurianti visus kaulus.
*/
void createCharacterBones(){
    joints.resize(JOINT_COUNT);
    joints[DUBUO] = {0, 0, 0, 0, 0, 0, (double)charStartingPos.x, (double)charStartingPos.y};

    actuallyCreateCharacterBones(K_KELIS, rad(88), rad(78), rad(180-78), thigh, true);
    actuallyCreateCharacterBones(D_KELIS, rad(92), rad(78), rad(180-78), thigh, false);
    actuallyCreateCharacterBones(K_PEDA, rad(88), rad(90), rad(180-90), calf, true);
    actuallyCreateCharacterBones(D_PEDA, rad(92), rad(90), rad(180-90), calf, false);

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
 * Ką pakeisti: teamhoode.
 * @return "KOJOS", kai veikėjas paliečia platformą. "-", kai veikėjas nieko nepaliečia.
*/
std::string collisions(){
    for(int i : feet_index){
        if(platformPoints.size() >= int(joints[i].y + 1)  *WIDTH -joints[i].x + WIDTH || joints[i].y + 2 >= HEIGHT-1){
            if(platformPoints[int(joints[i].y + 1) *WIDTH -joints[i].x + WIDTH] == 1 || joints[i].y + 2 >= HEIGHT-1){
                return "KOJOS";
            }
        }
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
    joints[K_KELIS].rotDir = true;
    joints[D_KELIS].rotDir = false;
    joints[K_PEDA].rotDir = true;
    joints[D_PEDA].rotDir = false;
    if(count == 4) allowedToChangeState = true;
    return;
}

//nepatinka šitas
void animate(){
    //Moving
    if(velX!=0){
        allowedToChangeState = false;
        double speed = velX, A;
        int indx[2] = {D_KELIS, K_KELIS};
        for(int i : indx){
            if(joints[i].rotDir){
                joints[i].ANGLE -= speed/45;
                if(velX>0) A = joints[i].minAngle;
                else A =  joints[i].maxAngle;
                if(velX*joints[i].ANGLE <= rad(A) * velX) joints[i].rotDir = false;
            }
            else{
                joints[i].ANGLE+=speed/45;
                if(velX>0) A = joints[i].minAngle;
                else A = joints[i].maxAngle;
                if(velX*joints[i].ANGLE >= rad(A)*velX) joints[i].rotDir = true;
            }
        }
        indx[0] = D_PEDA;
        indx[1] = K_PEDA;
        for(int i : indx){
            if(std::abs(joints[i+1].ANGLE - pi_2) < 0.01) begin[i] = true;
            if(joints[i].rotDir){
                begin[i] = false;
                joints[i].ANGLE -= speed/32;
                if(velX>0) A = joints[i].minAngle;
                else A = joints[i].maxAngle;
                if(velX*joints[i].ANGLE <= rad(A)*velX) joints[i].rotDir = false;
            }
            else if(begin[i]){
                joints[i].ANGLE+=speed/32;
                if(velX>0) A = joints[i].minAngle;
                else A = joints[i].maxAngle;
                if(velX*joints[i].ANGLE >= rad(A)*velX) joints[i].rotDir = true;
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
    SDL_Surface* tmpSurfc{};
    spritePositions.resize(JOINT_COUNT);
    sprites.resize(totalSpriteCategoryNumb);
    sprites[category_test].resize(categoryTest_SpriteNumb);

    tmpSurfc = SDL_LoadBMP("./assets/images/test/kojele.bmp");
    sprites[category_test][test_koja].img_data = tmpSurfc;
    sprites[category_test][test_koja].img = SDL_CreateTextureFromSurface(rend, tmpSurfc);

    SDL_FreeSurface(tmpSurfc); //free memory, kadangi vis tapatį variable naudojam, neturėtų būti memory leak, bet gali nutikti somehow vistiek 
    tmpSurfc = SDL_LoadBMP("./assets/images/test/kunelis.bmp");
    sprites[category_test][test_kunelis].img_data = tmpSurfc;
    sprites[category_test][test_kunelis].img = SDL_CreateTextureFromSurface(rend, tmpSurfc);
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
    debuglines();
    return;
}
//komentaras;