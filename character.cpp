#include"includes.h"
namespace petto{
    struct spr{
        //SDL_Surface *img;
        int w, h;
        SDL_Texture *txtr;
    };
    /**
     * `sprites` laikomos nuotraukos. 
     * Parent vektorius laiko kategorijas, o children yra pačios nuotraukos, 
     * surikiuotos 'eiliškumo' tvarka.
     * Eiliškumas, tai kaip 'layeriai', kuris sprite ant kurio turi būti, 
     * paskutiniai sprites bus virš pirmųjų.
     * 
     * Todėl structs(below) turi atitikti tą eiliškumą.
    */
    std::vector<std::vector<spr>> sprites;

    struct{
        int NUMB = 2;

        int PASUKTAS = 0;
        int STOVINTIS = 1;
    }CATEGORY;
    struct{
        int NUMB = 7;

        int AUSIS_GALE;
        int AUSIS_PRIEKYJE;
        int NEUTRALUS_GALVA;
        int KOJA_GALE;
        int KOJA_PRIEKYJE;
        int KUNELIS;
        int RANKA_PRIEKYJE;
    }PASUKTAS;
    struct{
        int NUMB = 5;

        int AUSYS;
        int NEUTRALUS_GALVA;
        int KOJOS;
        int KUNELIS;
        int RANKOS;
    }STOVINTIS;
    
    struct{
        int STOVINTIS = 0;
        int EINANTIS = 1;
    }ANIMATIONS;
    int PREV_anim = ANIMATIONS.STOVINTIS;
    int CURRENT_anim = ANIMATIONS.STOVINTIS;

    struct pasusisukimo_info{
        double pgr_kampas;
        double min_kampas;
        double max_kampas;
    };
    std::vector<double> kampai;
    std::vector<std::vector<pasusisukimo_info>> kampuInfo;
    //k_koja, d_koja, kunelis, k_ranka, d_ranka, galva, k_ausis, d_ausis.
    //   0      1        2         3        4       5      6        7

    //Stovinčio veikėjo animacija.
    void animate_STOVINTIS();
    //Einančio veikėjo animacija;
    void animate_EINANTIS();

    /**
     * 
    */
    void loadBMPs(std::string path, spr &sprite);
    
    //Funkcija iškviečianti tam tikrą animacijos funkciją.
    void animate();

//todo: add comments
    bool waitForBaseAngle();
    int stringToInt(std::string in);
    std::string getNthWord(std::string in, int n);
    double stringToDouble(std::string in);
    void getAngles(std::vector<pasusisukimo_info> &vektor, std::string path, bool saveToCurrent);
}

void petto::loadBMPs(std::string path, spr &sprite){
    //add prefix;
    if(path[0] != '.'){
        std::string temp = "./assets/images/";
        path = temp + path;
    }
    //add suffix
    if(path.size() < 4)
        path = path + ".bmp";
    else{
        std::string bmp;
        for(int i = path.size()-4; i < path.size(); i++)
            bmp+=path[i];
        if(bmp != ".bmp") path = path + ".bmp";
    }

    //Užkrauti nuotrauką ir išsaugoti informaciją.
    SDL_Surface *tmpSurfc = SDL_LoadBMP(path.c_str());
    sprite.w = tmpSurfc->w;
    sprite.h = tmpSurfc->h;
    sprite.txtr = SDL_CreateTextureFromSurface(rend, tmpSurfc);


    SDL_FreeSurface(tmpSurfc);
    return;
}

void petto::processCharacter(){
    //EMOTIONS

    //DECIDE WHERE TO GO

    animate();

    //move the body

    return;
}

void petto::animate(){
    //STOVINTIS
    if(CURRENT_anim == ANIMATIONS.STOVINTIS){

        if(PREV_anim == ANIMATIONS.EINANTIS){
            if(waitForBaseAngle())
                PREV_anim == ANIMATIONS.STOVINTIS;
        }

        if(PREV_anim == ANIMATIONS.STOVINTIS)
            animate_STOVINTIS();
    }

    //EINANTIS
    if(CURRENT_anim == ANIMATIONS.EINANTIS){

        if(PREV_anim == ANIMATIONS.EINANTIS || PREV_anim == ANIMATIONS.STOVINTIS)
            animate_EINANTIS();

    }

    return;
}

bool petto::waitForBaseAngle(){

    //true when the base angle reached
    return;
}

void petto::animate_STOVINTIS(){
    //INSTANT CHANGE
    //Stovincio pozicija, rotations, idle animation.

    return;
}

void petto::animate_EINANTIS(){

    return;
}

void petto::createCharacter(){
    sprites.resize(CATEGORY.NUMB);

    sprites[CATEGORY.PASUKTAS].resize(PASUKTAS.NUMB);
    loadBMPs("pasuktas/ausis_gale", sprites[CATEGORY.PASUKTAS][PASUKTAS.AUSIS_GALE]);
    loadBMPs("pasuktas/ausis_priekyje", sprites[CATEGORY.PASUKTAS][PASUKTAS.AUSIS_PRIEKYJE]);
    loadBMPs("pasuktas/neutralus_galva", sprites[CATEGORY.PASUKTAS][PASUKTAS.NEUTRALUS_GALVA]);
    loadBMPs("pasuktas/koja_gale", sprites[CATEGORY.PASUKTAS][PASUKTAS.KOJA_GALE]);
    loadBMPs("pasuktas/koja_priekyje", sprites[CATEGORY.PASUKTAS][PASUKTAS.KOJA_PRIEKYJE]);
    loadBMPs("pasuktas/ausis_kunelis", sprites[CATEGORY.PASUKTAS][PASUKTAS.KUNELIS]);
    loadBMPs("pasuktas/ranka_priekyje", sprites[CATEGORY.PASUKTAS][PASUKTAS.RANKA_PRIEKYJE]);

    sprites[CATEGORY.STOVINTIS].resize(STOVINTIS.NUMB);
    loadBMPs("stovintis/ausys", sprites[CATEGORY.STOVINTIS][STOVINTIS.AUSYS]);
    loadBMPs("stovintis/neutralus_galva", sprites[CATEGORY.STOVINTIS][STOVINTIS.NEUTRALUS_GALVA]);
    loadBMPs("stovintis/kojos", sprites[CATEGORY.STOVINTIS][STOVINTIS.KOJOS]);
    loadBMPs("stovintis/kunelis", sprites[CATEGORY.STOVINTIS][STOVINTIS.KUNELIS]);
    loadBMPs("stovintis/rankos", sprites[CATEGORY.STOVINTIS][STOVINTIS.RANKOS]);

    kampuInfo.resize(CATEGORY.NUMB);
    kampai.resize(7);

    getAngles(kampuInfo[CATEGORY.STOVINTIS], "stovintis", true);
    getAngles(kampuInfo[CATEGORY.PASUKTAS], "pasuktas", false);

    return;
}

void petto::getAngles(std::vector<pasusisukimo_info> &vektor, std::string path, bool saveToCurrent){
    vektor.resize(7);

    if(path[0] != '.')
        path = "./assets/animations/" + path;

    std::string tmp = "";
    for(int i = path.size()-4; i < path.size(); i++)
        tmp += path[i];
    if(tmp != ".txt")
        path += ".txt";

    std::ifstream animdata{path};
    
    while(!animdata.eof()){
        std::string line;
        std::getline(animdata, line);

        //index base min max
        vektor[stringToInt(getNthWord(line, 1))] = {stringToDouble(getNthWord(line, 2)), stringToDouble(getNthWord(line, 3)), stringToDouble(getNthWord(line, 4))};
        
        if(saveToCurrent)
            kampai[stringToInt(getNthWord(line, 1))] = stringToDouble(getNthWord(line, 2));
    }
    animdata.close();
    return;
}

int petto::stringToInt(std::string in){
    if(in.size() == 0) return 1;

    int _return = 0;
    int multiplier = 1;
    for(int i = in.size()-1; i >= 0; i--){
        if(i == 0 && in[i] == '-') _return*=-1;
        else if(i!=0 && in[i] == '-') return 0;

        _return += ((int)in[i]-48)*multiplier;
        multiplier*=10;
    }

    return _return;
}

std::string petto::getNthWord(std::string in, int n){
    std::string _return = "";
    int spaces = 0;
    for(int i = 0; i < in.size(); i++){
        if(in[i] == ' '){
            spaces++;
            if(spaces == n)
                return _return;
            
            _return = "";
        }
        else
            _return += in[i];
    }

    return _return;
}

//can be a dot or a comma.
double petto::stringToDouble(std::string in){
    double _return = 0;
    if(in.size() == 0) return 1;

    int k = 0;
    if(in[0] == '-') k = 1;
    double multiplier = 0.1;
    while(true){
        if(in.size() == k) break;
        if(in[k] == ',' || in[k] == '.')
            break;
        k++;
        multiplier*=10;
    }
    
    for(int i = 0; i<in.size(); i++){
        if(in[i] != '-' && in[i] != ',' && in[i] != '.'){
            _return += ((int)in[i]-48)*multiplier;
            multiplier/=10;
        }
    }

    if(in[0] == '-') _return*=-1;
    return _return;
}