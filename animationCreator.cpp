//i686-w64-mingw32-g++ -I src/include -L src/lib -o animator animationCreator.cpp  -lmingw32 -lSDL2main -lSDL2 -lfreetype

#include<SDL2/SDL.h>

#include<ft2build.h>
#include FT_FREETYPE_H

#include<iostream>
#include<vector>

int WIDTH = 1000;
int HEIGHT = 600;

FT_Library ft;
FT_FaceRec_* face;
int font_Size = 16;
SDL_Color colors[256];

SDL_Renderer* rend;
SDL_Event evt;

struct{
    SDL_Rect tmp = {20,20, 200, 15};

}boxes;

std::string intToString(int numb);
void loadFonts(int fontSize);
void renderText(std::string sentence, SDL_Rect &textBox, int fontMaxHeight);
void slider(std::string label, int fontSize, SDL_Rect &sliderBox, int &value, int minValue, int maxValue);
void loadSprites();

    struct spr{
        //SDL_Surface *img;
        int w, h;
        SDL_Texture *txtr;
    };
std::vector<spr> sprites;

int main(int argc, char *argv[]){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)) std::cout << "Failed to initialize SDL!\n";
    if(FT_Init_FreeType(&ft)) std::cout << "Failed to initialize FreeType library!\n";
    loadFonts(font_Size);

    SDL_Window* wind = SDL_CreateWindow("animator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    rend = SDL_CreateRenderer(wind, -1, 0);
    int tmpValue = 50;
    while(true)
    {
        SDL_PollEvent(&evt);
        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);
        SDL_SetRenderDrawColor(rend, 255,255,255,255);

        // * Possibly a flag to put everything in a single line?
        //renderText("Heyo daiud huwahd hawuihd ahwuidh iawdu hawidh uih", boxes.tmp, font_Size);
        
        //slider("skaiciai: $ * 0.1", font_Size, boxes.tmp, tmpValue, 0 ,100);
        loadSprites();

        if(evt.type == SDL_QUIT)
            break;
        SDL_RenderPresent(rend);
    }
    return 0;
}

void loadSprites(){


    return;
}

/**
 * Loads the main .ttf type font.
 * Considers these variables as global:
 * `FT_Library ft;`
 * `FT_FaceRec_* face;`
 * `int font_Size = 12;`
 * `SDL_Color colors[256];`
*/
void loadFonts(int fontSize){
    FT_Open_Args args;
    args.flags = FT_OPEN_PATHNAME;
    char fontpath[] = "./assets/fonts/OpenSans-Regular.ttf";
    args.pathname = fontpath; 
    if(FT_Open_Face(ft, &args, 0, &face)) std::cout << "Failed to load a font!\n";
    FT_Set_Pixel_Sizes(face, fontSize, fontSize);
    face->glyph->format = FT_GLYPH_FORMAT_BITMAP;

    for(int i = 0; i < 256; i++)
        colors[i].r = colors[i].g = colors[i].b = colors[i].a= i;
    return;
}

/**
 * Display text. Considers *rend as an existing global variable;
 * @param sentence the text to display.
 * @param textBox the SDL_Rect area where the text should be.
 * @param fontSize is the size of the font.
*/
void renderText(std::string sentence, SDL_Rect &textBox, int fontSize){
    std::vector<std::string> words_str;
    std::string individual_word{""};
    FT_Bitmap ftbitmap;

    for(int i = 0; i < sentence.size(); i++){
        if(sentence[i] == ' '){
            words_str.push_back(individual_word);
            individual_word = "";
        }
        else individual_word += sentence[i];
    }
    words_str.push_back(individual_word);

    //Display all characters.
    int totalWidth = 1;
    int y = 1;
    //Word info
    int letterCount = 0; //since the start of the word.

    for(std::string phrase : words_str){
        for(char letter : phrase){
            //Get bitmap
            FT_Load_Char(face, letter, FT_LOAD_RENDER);
            ftbitmap = face->glyph->bitmap;

            //Create a surface and apply palette's colros
            SDL_Surface* glyph = SDL_CreateRGBSurfaceFrom(ftbitmap.buffer, ftbitmap.width, ftbitmap.rows, 8, ftbitmap.pitch, 0, 0, 0, 0xFF);
            SDL_SetPaletteColors(glyph->format->palette, colors, 0, 256);
            SDL_SetSurfaceBlendMode(glyph, SDL_BlendMode::SDL_BLENDMODE_ADD);            

            //Create the 'letterbox'.
            int belowBaseLine = (face->glyph->metrics.height - face->glyph->metrics.horiBearingY)/55;    
            SDL_Rect pos = {textBox.x + totalWidth, int(textBox.y + y + fontSize - glyph->h + belowBaseLine), glyph->w, glyph->h};
            
            //Check if the letter goes to a new line.
            totalWidth+=glyph->w;
            if(totalWidth>=textBox.w){
                y += fontSize;
                totalWidth = 1 + glyph->w;
                pos.x = textBox.x + 1;
                pos.y = int(textBox.y + y + fontSize - glyph->h + belowBaseLine);
            }

            //Display the letter.
            SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, glyph);
            SDL_RenderCopy(rend, texture, NULL, &pos);
            
            //Clean up.
            SDL_FreeSurface(glyph);
            SDL_DestroyTexture(texture);
        }
        totalWidth+=fontSize/4;
        letterCount = 0;
    }
    return;
}

/**
 * Creats a slider inside SDL_Rect;
 * Assumes that `*rend` and `evt` exists as global variables.
 * In `label` variable, symbol `$` will be replaced with `value`.
 * Uses `renderText` function.
*/
void slider(std::string label, int fontSize, SDL_Rect &sliderBox, int &value, int minValue, int maxValue){
    int slider_mx, slider_my;
    int notch_width = sliderBox.h, notch_height = sliderBox.h;

    SDL_RenderDrawLine(rend, sliderBox.x, sliderBox.y + sliderBox.h/2, sliderBox.x + sliderBox.w - 1, sliderBox.y + sliderBox.h/2);

    if(SDL_GetMouseState(&slider_mx, &slider_my) & SDL_BUTTON_LMASK && slider_mx >= sliderBox.x && slider_mx < sliderBox.x + sliderBox.w && slider_my >= sliderBox.y && slider_my < sliderBox.y + sliderBox.h){
        value = (slider_mx-sliderBox.x-notch_width/2)*(maxValue-minValue+notch_width/2)/sliderBox.w;
        if(value < minValue) value = minValue;
        if(value > maxValue) value = maxValue;
    }

    SDL_Rect slider_textbox = {sliderBox.x, sliderBox.y-fontSize-2, sliderBox.w, fontSize};
    
    std::string new_label = "";
    for(int i = 0; i < label.size(); i++){
        if(label[i] == '$')
            new_label+=intToString(value);
        else 
            new_label+=label[i];
    }
    renderText(new_label, slider_textbox, fontSize);
    
    SDL_Rect slider_notch = {value*sliderBox.w/(maxValue-minValue+notch_width/2)+sliderBox.x, sliderBox.y+sliderBox.h/2-notch_height/2+1, notch_width, notch_height};
    SDL_RenderDrawRect(rend, &slider_notch);
    return;
}

std::string intToString(int numb){
    std::string _return_backwards = "";

    if(numb == 0)
        _return_backwards += "0";
    while(numb != 0){
        _return_backwards += char(numb%10+48);
        numb/=10;
    }

    std::string _return = "";
    for(int i = _return_backwards.size()-1; i>=0; i--)
        _return+=_return_backwards[i];

    return _return;
}