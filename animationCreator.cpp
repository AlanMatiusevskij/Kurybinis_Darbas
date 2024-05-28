//i686-w64-mingw32-g++ -I src/include -L src/lib -o animator animationCreator.cpp  -lmingw32 -lSDL2main -lSDL2 -lfreetype
//this is a mess. redo it all 
#include<SDL2/SDL.h>

#include<ft2build.h>
#include FT_FREETYPE_H

#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include<filesystem>

int WIDTH = 1000;
int HEIGHT = 600;

FT_Library ft;
FT_FaceRec_* face;
SDL_Color colors[256];

SDL_Window *wind;
SDL_Renderer *rend;
SDL_Event evt;

std::string CD = "./";

std::string intToString(int numb);
void loadFont(int fontSize);
void loadSprites();
void browseDirectory(std::string &cd);

void renderText(std::string sentence, SDL_Rect textBox, int fontSize);
void slider(std::string label, int fontSize, SDL_Rect sliderBox, int &value, int minValue, int maxValue);
void button(std::string label, SDL_Rect buttonbox, void(*onClick)());

struct spr{
    int rotation;
    int x, y;
    int w, h;
    SDL_Texture *txtr;
};
std::vector<spr> sprites;

struct save_info{
    std::string sprite_category, sprite_name;
    int rotation;
    int realtive_x, relative_y;
    int dividend;
};

void place();
void render();
void saveToFile();
void clearFrames();
void saveFrame();

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
    FT_Init_FreeType(&ft);

    wind = SDL_CreateWindow("animator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    loadSprites();

    while(true){
        SDL_PollEvent(&evt);
        

        if(evt.type == SDL_QUIT)
            break;
    }

    SDL_Quit();
    return 0;
}

void browseDirectory(std::string &cd){

}

void loadSprites(){
    std::string path = "./assets/images/stovintis";
    for(const auto &entry : std::filesystem::directory_iterator(path)){
        if(entry.path().extension() == ".bmp")
            std::cout << entry.path().filename() << "\n";
    }
}