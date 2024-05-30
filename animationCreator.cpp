//i686-w64-mingw32-g++ -I src/include -L src/lib -o animator animationCreator.cpp  -lmingw32 -lSDL2main -lSDL2 -lfreetype
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

FT_FaceRec_ *face;
std::string CD = "./assets/images";

enum GUItypes{
    TEXT = 0,
    BUTTON = 1
};

std::string intToString(int numb);
void loadFont(int fontSize);
void loadSprites();
void selectDirectory(std::string label);
void browseDirectory(std::string &cd, SDL_Rect box, int fontSize);
void cdBack();

void renderText(std::string sentence, SDL_Rect textBox, int fontSize, bool newLines);
void slider(std::string label, int fontSize, SDL_Rect sliderBox, int &value, int minValue, int maxValue);
void button(std::string label, SDL_Rect buttonbox, int fontSize, void(*onClick)());
void scrollBar(GUItypes type, SDL_Rect box, std::vector<std::string> entries, int fontSize, void (*onClick)(std::string));

bool onRect(SDL_Rect rect);

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

void selectDirectory();
void saveToFile();

void clearFrames();
void saveFrame();

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
    FT_Init_FreeType(&ft);

    wind = SDL_CreateWindow("animator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    rend = SDL_CreateRenderer(wind, -1, 0);
    SDL_SetRenderDrawBlendMode(rend, SDL_BlendMode::SDL_BLENDMODE_BLEND);

    for(int i = 0; i < 256; i++)
        colors[i].r = colors[i].g = colors[i].b = colors[i].a= i;
         
    while(true){
        SDL_PollEvent(&evt);
        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);
        SDL_SetRenderDrawColor(rend, 255,255,255,255);

        browseDirectory(CD, {10, 10, 300, 300}, 18);

        if(evt.type == SDL_QUIT)
            break;
        SDL_RenderPresent(rend);
    }

    FT_Done_FreeType(ft);
    SDL_Quit();
    return 0;
}

void button(std::string label, SDL_Rect buttonbox, int fontSize, void(*onClick)()){
    if(onRect(buttonbox) && evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT)
        onClick();
    renderText(label, buttonbox, fontSize, false);
}

void browseDirectory(std::string &cd, SDL_Rect box, int fontSize){
    //Get current files in the directory TODO: error handling
    std::vector<std::string> files;
    for(const auto &entry : std::filesystem::directory_iterator(cd)){
        files.push_back(entry.path().generic_string());
    }

    //Draw boxes
    SDL_RenderDrawRect(rend, &box);
    SDL_RenderDrawLine(rend, box.x, box.y + fontSize + 2, box.x + box.w - 1, box.y + fontSize + 2);

    button("..", {box.x + box.w - fontSize, box.y, fontSize, fontSize}, fontSize, &cdBack);

    //Print current directory
    renderText(cd, {box.x + 1, box.y + 1, box.w - 1, fontSize}, fontSize, false);

    scrollBar(GUItypes::BUTTON, {box.x, box.y + fontSize + 2, box.w, box.h -fontSize-2}, files, fontSize, &selectDirectory);
}

void cdBack(){
    if(CD.size() > 2 && CD[CD.size()-1] == '/') CD.pop_back();
    for(int i = CD.size()-1; i >= 1; i--){
        if(CD[i] != '/') CD.pop_back();
        else
            return;
    }
}

bool onRect(SDL_Rect rect){
    int tmpMx, tmpMy;
    SDL_GetMouseState(&tmpMx, &tmpMy);
    if(rect.x <= tmpMx && rect.x + rect.w > tmpMx && rect.y <= tmpMy && rect.y + rect.h > tmpMy)
        return true;
    return false;
}

std::string reverseString(std::string in){
    std::string _return = "";
    for(int i = in.size() -1; i >= 0; i--)
        _return += in[i];
    return _return;
}

void selectDirectory(std::string label){
    //Check if its a folder.
    std::string extension = "";
    bool a_folder = true;

    for(int i = label.size() - 1; i > 0; i--){
        extension+=label[i];
        if(label[i] == '/') break;
        if(label[i] == '.'){
            extension = reverseString(extension);
            a_folder = false;
            break;
        }
    }    
    if(a_folder) CD = label;
    //else if extension == ".bmp"// load an image
    return;
}

void scrollBar(GUItypes type, SDL_Rect box, std::vector<std::string> entries, int fontSize, void (*onClick)(std::string)){
    int y;
    static int delta;
    static bool scrollBarPressed = false;

    //shift is scroll bar position;
    static int shift = 0; 

    //Check if new entries
    static std::vector<std::string> old{};
    if(old != entries){
        old = entries;
        shift = 0;
    }

    //Display the bar slot
    SDL_Rect sliderSlot = {box.x + box.w - 8, box.y, 8, box.h};
    SDL_RenderDrawRect(rend, &sliderSlot);

    //Calculate the heigth of the bar and display it;
    if((1 + fontSize*entries.size()) > box.h){
        double height = (double(box.h*box.h)/((1+fontSize)*(entries.size()+2)));

        SDL_Rect sliderBox = {box.x + box.w - 8, box.y + shift, 8, (int)height};
        SDL_SetRenderDrawColor(rend, 0,0,0,140);
        SDL_RenderFillRect(rend, &sliderBox);

    //Scroll Functionallity
        if(evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT && onRect(sliderBox)){
            scrollBarPressed = true;
            SDL_GetMouseState(NULL, &delta);
        }
        if(evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LEFT)
            scrollBarPressed = false;

        if(scrollBarPressed){
            SDL_GetMouseState(NULL, &y);
            shift += y - delta;
            delta = y;
            if(shift < 0) shift = 0;
            if(shift+(int)height > box.h) shift = box.h - (int)height;
        }
    }

    //Show directories and files
    if(type == GUItypes::BUTTON){
        for(int i = 0; i < entries.size(); i++){
            int y_position = box.y + 1 + fontSize*i-shift*((fontSize*entries.size())/box.h);

            if(box.y-y_position <= 0){
                if(y_position - fontSize> box.h) break;
                SDL_Rect butBox = {box.x, y_position, box.w, fontSize};
                if(onRect(butBox) && evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT)
                onClick(entries[i]);
                renderText(entries[i], butBox, fontSize, false);
            }
        }
    }
    return;
}

void loadSprites(){
    std::string path = "./assets/images/stovintis";
    for(const auto &entry : std::filesystem::directory_iterator(path)){
        if(entry.path().extension() == ".bmp")
            std::cout << entry.path().filename() << "\n";
    }

}

void loadFont(int fontSize){
    FT_Open_Args args;
    args.flags = FT_OPEN_PATHNAME;
    char fontpath[]= "./assets/fonts/OpenSans-Regular.ttf";
    args.pathname = fontpath;
    if(FT_Open_Face(ft, &args, 0, &face)) std::cout << "Failed to load fonts!\n";
    face->glyph->format = FT_GLYPH_FORMAT_BITMAP;
    FT_Set_Pixel_Sizes(face, fontSize, fontSize);
}

void renderText(std::string sentence, SDL_Rect textBox, int fontSize, bool newLines){
    std::vector<std::string> words;
    std::string ind_word{""};
    FT_Bitmap ftbitmap;

    loadFont(fontSize);

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
            SDL_Rect pos = {textBox.x + totalWidth, int(textBox.y + linePos + (fontSize-fontSize/5) - glyph->h + belowBaseLine), glyph->w, glyph->h};

            totalWidth+=glyph->w;
            if(newLines && totalWidth >= textBox.w){
                linePos+=fontSize;
                totalWidth = 1 + glyph->w;
                pos.x = textBox.w + 1;
                pos.y = int(textBox.y + linePos + (fontSize-fontSize/5) - glyph->h + belowBaseLine);
            }

            //Display the symbol
            SDL_Texture *txtr = SDL_CreateTextureFromSurface(rend, glyph);
            //Don't render outside the bounds!
            SDL_Rect renderArea = {0, 0, std::min(textBox.w-totalWidth, glyph->w), std::min(textBox.h-linePos, glyph->h)};
            SDL_RenderCopy(rend, txtr, &renderArea, &pos);

            //clean up
            SDL_FreeSurface(glyph);
            SDL_DestroyTexture(txtr);
        }
        totalWidth+=fontSize/4;
    }
    FT_Done_Face(face);
}