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
        int NUMB = 8;

        int AUSIS_GALE;
        int AUSIS_PRIEKYJE;
        int GALVA;
        int KOJA_GALE;
        int KOJA_PRIEKYJE;
        int KUNELIS;
        int RANKA_PRIEKYJE;
        int NEUTRALUS_VEIDAS;
    }PASUKTAS;
    struct{
        int NUMB = 6;

        int AUSYS;
        int GALVA;
        int KOJOS;
        int KUNELIS;
        int NEUTRALUS_VEIDAS;
        int RANKOS;
    }STOVINTIS;

    /**
     * 
    */
    void petto::loadBMPs(std::string path, spr &sprite);
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

    //use a big brain method;

    //move the body

    return;
}

void petto::createCharacter(){
    sprites.resize(CATEGORY.NUMB);

    sprites[CATEGORY.PASUKTAS].resize(PASUKTAS.NUMB);
    loadBMPs("pasuktas/ausis_gale", sprites[CATEGORY.PASUKTAS][PASUKTAS.AUSIS_GALE]);
    loadBMPs("pasuktas/ausis_priekyje", sprites[CATEGORY.PASUKTAS][PASUKTAS.AUSIS_PRIEKYJE]);
    loadBMPs("pasuktas/galva", sprites[CATEGORY.PASUKTAS][PASUKTAS.AUSIS_PRIEKYJE]);
    loadBMPs("pasuktas/koja_gale", sprites[CATEGORY.PASUKTAS][PASUKTAS.KOJA_GALE]);
    loadBMPs("pasuktas/koja_priekyje", sprites[CATEGORY.PASUKTAS][PASUKTAS.KOJA_PRIEKYJE]);
    loadBMPs("pasuktas/ausis_kunelis", sprites[CATEGORY.PASUKTAS][PASUKTAS.KUNELIS]);
    loadBMPs("pasuktas/neutralus_veidas", sprites[CATEGORY.PASUKTAS][PASUKTAS.NEUTRALUS_VEIDAS]);
    loadBMPs("pasuktas/ranka_priekyje", sprites[CATEGORY.PASUKTAS][PASUKTAS.RANKA_PRIEKYJE]);

    sprites[CATEGORY.STOVINTIS].resize(STOVINTIS.NUMB);
    loadBMPs("stovintis/ausys", sprites[CATEGORY.STOVINTIS][STOVINTIS.AUSYS]);
    loadBMPs("stovintis/galva", sprites[CATEGORY.STOVINTIS][STOVINTIS.GALVA]);
    loadBMPs("stovintis/kojos", sprites[CATEGORY.STOVINTIS][STOVINTIS.KOJOS]);
    loadBMPs("stovintis/kunelis", sprites[CATEGORY.STOVINTIS][STOVINTIS.KUNELIS]);
    loadBMPs("stovintis/neutralus_veidas", sprites[CATEGORY.STOVINTIS][STOVINTIS.NEUTRALUS_VEIDAS]);
    loadBMPs("stovintis/rankos", sprites[CATEGORY.STOVINTIS][STOVINTIS.RANKOS]);

    return;
}