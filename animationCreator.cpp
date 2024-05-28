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
SDL_Color colors[256];

SDL_Window *wind;
SDL_Renderer *rend;
SDL_Event evt;

std::string CD = "./";

std::string intToString(int numb);
void loadFont(int fontSize, FT_FaceRec_ *face);
void loadSprites();
void browseDirectory(std::string &cd);

void renderText(std::string sentence, SDL_Rect textBox, int fontSize, bool newLines);
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

enum GUItypes{
    TEXT = 0,
    BUTTON = 1
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

    for(int i = 0; i < 256; i++)
        colors[i].r = colors[i].g = colors[i].b = colors[i].a= i;
    loadSprites();

    while(true){
        SDL_PollEvent(&evt);
        

        if(evt.type == SDL_QUIT)
            break;
    }

    FT_Done_FreeType(ft);
    SDL_Quit();
    return 0;
}

void browseDirectory(std::string &cd){
    //Get current files in the directory
    std::vector<std::string> files;
    for(const auto &entry : std::filesystem::directory_iterator(cd)){
        files.push_back(entry.path().filename().generic_string());
    }


}

void scrollBar(GUItypes type, SDL_Rect box, std::vector<std::string> entries, void(*onButtonClick)()){
    
}

void loadSprites(){
    std::string path = "./assets/images/stovintis";
    for(const auto &entry : std::filesystem::directory_iterator(path)){
        if(entry.path().extension() == ".bmp")
            std::cout << entry.path().filename() << "\n";
    }
}

void loadFont(int fontSize, FT_FaceRec_ *face){
    FT_Open_Args args;
    args.flags = FT_OPEN_PATHNAME;
    char fontpath[]= "./assets/fonts/OpenSans-Regular.ttf";
    args.pathname = fontpath;
    if(FT_Open_Face(ft, &args, 0, &face)) std::cout << "Failed to load fonts!\n";
    FT_Set_Pixel_Sizes(face, fontSize, fontSize);
    face->glyph->format = ft_glyph_format_bitmap;
}

void renderText(std::string sentence, SDL_Rect textBox, int fontSize, bool newLines){
    std::vector<std::string> words;
    std::string ind_word{""};
    FT_Bitmap ftbitmap;
    FT_FaceRec_* face;

    loadFont(fontSize, face);

    //Save each word and whitespace in a vector
    for(int i = 0; i < sentence.size(); i++){
        if(sentence[i] == ' '){
            words.push_back(ind_word);
            ind_word = "";
        }
        else ind_word += sentence[i];
    }
    //save the last one
    words.push_back(ind_word);

    //Display all characters
    int totalWidth = 1;
    int linePos = 1;

    for(std::string word : words){
        for(char symb : word){
            FT_Load_Char(face, symb, FT_LOAD_RENDER);
            ftbitmap = face->glyph->bitmap;

            //Create a surface and apply palette's colros
            SDL_Surface* glyph = SDL_CreateRGBSurfaceFrom(ftbitmap.buffer, ftbitmap.width, ftbitmap.rows, 8, ftbitmap.pitch, 0, 0, 0, 0xFF);
            SDL_SetPaletteColors(glyph->format->palette, colors, 0, 256);
            SDL_SetSurfaceBlendMode(glyph, SDL_BlendMode::SDL_BLENDMODE_ADD);    

            //Letterbox
            int belowBaseLine = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY)/55;
            SDL_Rect pos = {textBox.x + totalWidth, int(textBox.y + linePos - fontSize + glyph->h + belowBaseLine), glyph->w, glyph->h};

            totalWidth+=glyph->w;
            if(newLines && totalWidth >= textBox.w){
                linePos+=fontSize;
                totalWidth = 1 + glyph->w;
                pos.x = textBox.w + 1;
                pos.y = int(textBox.y + linePos + fontSize - glyph->h + belowBaseLine);
            }

            //Display the symbol
            SDL_Texture *txtr = SDL_CreateTextureFromSurface(rend, glyph);
            //Don't render outside the bounds!
            SDL_Rect renderArea = {0, 0, std::min(textBox.w-totalWidth, glyph->w), std::min(textBox.h-linePos, glyph->h)};
            SDL_RenderCopy(rend, txtr, &renderArea, &textBox);

            //clean up
            SDL_FreeSurface(glyph);
            SDL_DestroyTexture(txtr);
        }
        totalWidth+=fontSize/4;
    }
}