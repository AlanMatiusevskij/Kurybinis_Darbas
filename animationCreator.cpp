//i686-w64-mingw32-g++ -I src/include -L src/lib -o animator animationCreator.cpp  -lmingw32 -lSDL2main -lSDL2 -lfreetype
#include<SDL2/SDL.h>

#include<ft2build.h>
#include FT_FREETYPE_H

#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include<cmath>
#include<bitset>
#include<filesystem>
#include<algorithm>

int WIDTH = 1600;
int HEIGHT = 800;
int UPS = 60;

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

struct transform_struct{
    int x;
    int y;
    int w;
    int h;
    int scale_x = 1;
    int scale_y = 1;
    int angle = 0;
    //SDL_Point rotationCenter;
    //#SDL_FLIP status?
};

struct sprite_struct{
    //order in layers?
    std::string path;
    transform_struct transform;
    std::vector<int8_t> alphas;
    SDL_Texture *texture;
    SDL_Texture *transparencyMask;
};
//think of them as layered images
std::vector<sprite_struct> sprites;

void renderSprites(bool transparencyMask);
void moveSprites();

void loadBMP(std::string path);
void loadFont(int fontSize);

void selectDirectory(std::string label);
void browseDirectory(std::string &cd, SDL_Rect box, int fontSize);
void cdBack();  

std::string selectedSprite = "";
void inspector(SDL_Rect box, int fontSize);
void selectActiveSprite(std::string in);

bool onRect(SDL_Rect rect);
std::string intToString(int numb);

class UI{
public:
    //UI
    void renderText(std::string sentence, SDL_Rect textBox, int fontSize, bool newLines);
    void slider(std::string label, int fontSize, SDL_Rect sliderBox, int &value, int minValue, int maxValue);
    void button(std::string label, SDL_Rect buttonbox, int fontSize, void(*onClick)());
    void scrollBox(GUItypes type, SDL_Rect box, std::vector<std::string> &entries, int fontSize, void (*onClick)(std::string));
private:
    int delta;
    bool scrollBarPressed = false;

    //shift is scroll bar position;
    int shift = 0; 

    //Check if new entries
    std::vector<std::string> old{};
};
UI explorer_class;
UI inspector_class;

struct save_info{
    std::string sprite_category, sprite_name;
    int rotation;
    int realtive_x, relative_y;
    int dividend;
};

void timeline();
void clearFrames();
void saveFrame();

void saveToFile();

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

        browseDirectory(CD, {10, 10, 270, 200}, 18);
        inspector({WIDTH-WIDTH/5, 30, WIDTH/5 - 10, HEIGHT - 60}, 18);

        moveSprites();
        renderSprites(false);

        if(evt.type == SDL_QUIT)
            break;
        SDL_RenderPresent(rend);
        SDL_Delay(1000/float(UPS));
    }

    FT_Done_FreeType(ft);
    SDL_Quit();
    return 0;
}

void UI::button(std::string label, SDL_Rect buttonbox, int fontSize, void(*onClick)()){
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

    //cdBack
    explorer_class.button("...", {box.x + box.w - fontSize, box.y, fontSize, fontSize}, fontSize, &cdBack);

    //Print current directory
    explorer_class.renderText(cd, {box.x + 1, box.y + 1, box.w - 1, fontSize}, fontSize, false);

    explorer_class.scrollBox(GUItypes::BUTTON, {box.x, box.y + fontSize + 2, box.w, box.h -fontSize-2}, files, fontSize, &selectDirectory);
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

    //Check for some exceptions
    if(label == "./Makefile") return;

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
    else 
        if(extension == ".bmp")
            loadBMP(label);
    return;
}

void loadBMP(std::string path){    
    //check if such one already exists
    for(sprite_struct &obj : sprites)
        if(obj.path == path)
            return;

    //Load surface and save information
    SDL_Surface *surf = SDL_LoadBMP(path.c_str());
    sprites.push_back({path, {WIDTH/2-surf->w/2, HEIGHT/2 - surf->h/2, surf->w, surf->h}, std::vector<int8_t>(std::ceil(float(surf->w*surf->h)/8)), SDL_CreateTextureFromSurface(rend, surf)});
    
    //Surface for creating transparency mask.
    SDL_Surface *transpMask = SDL_CreateRGBSurfaceWithFormat(0, surf->w, surf->h, 8*4, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA32);
    SDL_LockSurface(transpMask);

    //Save all 'intangible' points
    //by filling a vector with 11111111's, indicating non_complete_transparency.
    std::fill(sprites[sprites.size()-1].alphas.begin(), sprites[sprites.size()-1].alphas.end(), 255);


    //And finding all points where the image is completely transparent and change vector[byte][bit] value to 0, indicating complete_transparency.
    SDL_LockSurface(surf);
    int *red{new int(0)}, *green{new int(0)}, *blue{new int(0)}, *alfa{new int(0)};
    for(int _y = 0; _y < surf->h; _y++)
        for(int _x = 0; _x < surf->w; _x++){
            Uint32 _colorValues = *(Uint32*)((Uint8*)((Uint8*)surf->pixels + _y*surf->pitch + _x*surf->format->BytesPerPixel));
            SDL_GetRGBA(_colorValues, surf->format, (Uint8*)red, (Uint8*)green, (Uint8*)blue, (Uint8*)alfa);

            Uint8 *pixel = (Uint8*)transpMask->pixels;
            pixel+=_y*transpMask->pitch+_x*transpMask->format->BytesPerPixel;

            if(*alfa == 0){
                int byteNumb = (_y*surf->w + _x)/8;
                int bitNumb = (_y*surf->w + _x)%8;
                std::string binary = std::bitset<8>(sprites[sprites.size()-1].alphas[byteNumb]).to_string();
                binary[bitNumb] = '0';
                sprites[sprites.size()-1].alphas[byteNumb] = (int8_t)std::bitset<8>(binary).to_ulong();
                //                                  R                G                B                  A
                *((Uint32*)pixel) = (Uint32)((Uint8)255 << 0 | (Uint8)0 << 8 | (Uint8)0 << 16 | (Uint8)100 << 24);
            }
            else
                *((Uint32*)pixel) = (Uint32)((Uint8)0 << 0 | (Uint8)255 << 8 | (Uint8)0 << 16 | (Uint8)100 << 24);
        }
    SDL_UnlockSurface(transpMask);
    sprites[sprites.size()-1].transparencyMask = SDL_CreateTextureFromSurface(rend, transpMask);

    //Clean up
    delete red, green, blue, alfa;
    SDL_UnlockSurface(surf);
    SDL_FreeSurface(transpMask);
    SDL_FreeSurface(surf);
}

void renderSprites(bool transparencyMask){
    for(sprite_struct &obj : sprites){
        SDL_Rect pos = {obj.transform.x, obj.transform.y, obj.transform.w, obj.transform.h};
        SDL_RenderCopyEx(rend, obj.texture, NULL, &pos, obj.transform.angle, NULL, SDL_FLIP_NONE);
        if(transparencyMask) SDL_RenderCopyEx(rend, obj.transparencyMask, NULL, &pos, obj.transform.angle, NULL, SDL_FLIP_NONE);
    }
}

void UI::scrollBox(GUItypes type, SDL_Rect box, std::vector<std::string> &entries, int fontSize, void (*onClick)(std::string)){
    int sliderWidth = 8;
    int y;

    if(old != entries){
        old = entries;
        shift = 0;
    }

    //Display the bar slot
    SDL_Rect sliderBox;
    SDL_Rect sliderSlot = {box.x + box.w - sliderWidth, box.y, sliderWidth, box.h};
    SDL_RenderDrawRect(rend, &sliderSlot);

    //Calculate the heigth of the bar and display it;
    if((1 + fontSize*entries.size()) > box.h){
        double height = (double(box.h*box.h)/((1+fontSize)*(entries.size()+2)));

        sliderBox = {box.x + box.w - sliderWidth, box.y + shift, sliderWidth, (int)height};
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
            int y_position = box.y + 1 - (double(shift)/box.h)*fontSize*entries.size() + fontSize*i;

            if(box.y-y_position <= 0){
                if(y_position - fontSize> box.h) break;
                SDL_Rect butBox = {box.x, y_position, box.w-sliderWidth, fontSize};
                if(onRect(butBox) && evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT)
                    onClick(entries[i]);
                renderText(entries[i], butBox, fontSize, false);
            }
        }
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

void UI::renderText(std::string sentence, SDL_Rect textBox, int fontSize, bool newLines){
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

bool isOnTransparentPoint(sprite_struct &obj){
    int x, y;
    SDL_GetMouseState(&x, &y);

    int indx = (x-obj.transform.x)+(y-obj.transform.y)*obj.transform.w;
    if(!onRect({obj.transform.x, obj.transform.y, obj.transform.w, obj.transform.h})) return false;
    if(std::bitset<8>(obj.alphas[indx/8]).to_string()[indx%8] == '1') return true;
    return false;
}

void moveSprites(){
    static bool onButton = false;
    static int index = 0;
    static int prev_x, prev_y;
    int c_x, c_y;
    SDL_GetMouseState(&c_x, &c_y);

    for(int i = sprites.size()-1; i>=0; i--){
        if(isOnTransparentPoint(sprites[i]) && evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT){
            prev_x = c_x;
            prev_y = c_y;
            onButton = true;
            index = i;
            break;
        }
        if(evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LEFT){
            onButton = false;            
            break;
        }
    }

    if(onButton){
        sprites[index].transform.x += c_x-prev_x;
        sprites[index].transform.y += c_y-prev_y;

        prev_x = c_x;
        prev_y = c_y;
    }
}

void selectActiveSprite(std::string in){
    selectedSprite = in;
}

void inspector(SDL_Rect box, int fontSize){
    int x, y;
    SDL_GetMouseState(&x, &y);
    
    //Borders and lines
    SDL_SetRenderDrawColor(rend, 255,255,255,255);
    inspector_class.renderText("Inspector", {box.x + 2, box.y - fontSize - 3, box.w, fontSize}, fontSize, false);
    SDL_RenderDrawRect(rend, &box);
    SDL_RenderDrawLine(rend, box.x, box.y + box.h*2/10-1, box.x + box.w-1, box.y + box.h*2/10-1);

    //Create a scroll box:
    //Get all loaded paths into a string.
    std::vector<std::string> loaded;
    for(sprite_struct &obj : sprites)
        loaded.push_back(obj.path);
    if(loaded.size() == 0){
        inspector_class.renderText("Active sprites.", {box.x + 2, box.y + 5, box.w - 2, fontSize+4}, fontSize+4, false);
        inspector_class.renderText("Select a .bmp file from the explorer to open one.", {box.x + 2, box.y + 7 + fontSize+4, box.w - 2, fontSize-4}, fontSize-4, false);
    }
    else inspector_class.scrollBox(GUItypes::BUTTON, {box.x, box.y, box.w, box.h*2/10}, loaded, fontSize-4, &selectActiveSprite);

    //Details about the selected sprite.
    int detailsAreaY = box.y+box.h*2/10-1+12;
    std::string selected = "";
    for(int i = selectedSprite.size()-1; i >0; i--)
        if(selectedSprite[i] == '/') break;
        else selected+=selectedSprite[i];
    selected = reverseString(selected);
    
    inspector_class.renderText(selected, {box.x + 20, detailsAreaY, box.w - 20, fontSize}, fontSize, false);
}