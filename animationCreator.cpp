//i686-w64-mingw32-g++ -I src/include -L src/lib -o animator animationCreator.cpp  -lmingw32 -lSDL2main -lSDL2 -lfreetype

#include<SDL2/SDL.h>

#include<ft2build.h>
#include FT_FREETYPE_H

#include<fstream>
#include<iostream>
#include<vector>

int WIDTH = 1000;
int HEIGHT = 600;

FT_Library ft;
FT_FaceRec_* face;
int font_Size = 12;
SDL_Color colors[256];

SDL_Renderer* rend;
SDL_Event evt;


int spriteNumb = 6;

std::string intToString(int numb);
void loadFonts(int fontSize);
void renderText(std::string sentence, SDL_Rect textBox, int fontMaxHeight);
void slider(std::string label, int fontSize, SDL_Rect sliderBox, int &value, int minValue, int maxValue);
//`*textBox` can be a NULL and the text will be put inside `buttonBox`.
void button(std::string label, SDL_Rect textBox, SDL_Rect buttonBox, void (*onClick)());
void loadSprites();

void selected0();
void selected1();
void selected2();
void selected3();
void selected4();
void selected5();

struct spr{
    int rotation;
    int x, y;
    int w, h;
    SDL_Texture *txtr;
};
std::vector<spr> sprites;
int currentSelected = -1;

struct save_info{
    int rotation;
    int relative_x, relative_y;
    int dividend; //?
};
std::vector<std::vector<save_info>> info;

bool pressedAButton = false;
void placeRelative();
void render(int i);
void save();
void clearInfo();
void capture();

int main(int argc, char *argv[]){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)) std::cout << "Failed to initialize SDL!\n";
    if(FT_Init_FreeType(&ft)) std::cout << "Failed to initialize FreeType library!\n";
    loadFonts(font_Size);
    info.resize(spriteNumb);
    SDL_Window* wind = SDL_CreateWindow("animator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    rend = SDL_CreateRenderer(wind, -1, 0);

    loadSprites();

    while(true)
    {
        SDL_PollEvent(&evt);
        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);
        SDL_SetRenderDrawColor(rend, 255,255,255,255);

        //renderText("Heyo", boxes.tmp, font_Size);
        //slider("skaiciai: $", font_Size, boxes.tmp, tmpValue, 0 ,100);
        //button("++", &boxes.textArea, boxes.button, &test);

        if(evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LEFT) pressedAButton = false;

        button("d_ausis", {21, 30, 40, font_Size}, {20, 20, 40, 40}, &selected0);
        button("k_ausis", {21, 80, 40, font_Size}, {20, 70, 40, 40}, &selected1);
        button("kojos", {21, 130, 40, font_Size}, {20, 120, 40, 40}, &selected2);
        button("kunelis", {21, 180, 40, font_Size}, {20, 170, 40, 40}, &selected3);
        button("galva", {21, 230, 40, font_Size}, {20, 220, 40, 40}, &selected4);
        button("rankos", {21, 280, 40, font_Size}, {20, 270, 40, 40}, &selected5);

        button("capture", {WIDTH-45, 20, 40, font_Size}, {WIDTH-50, 10, 40, 40}, &capture);
        button("clear", {WIDTH-45, 60, 40, font_Size}, {WIDTH-50, 50, 40, 40}, &clearInfo);
        button("save", {WIDTH-45, 100, 40, font_Size}, {WIDTH-50, 90, 40, 40}, &save);

        slider("kampas d_ausis: $", font_Size , {100, 20, 200,20}, sprites[0].rotation, 0, 720);
        slider("kampas k_ausis: $", font_Size , {100, 60, 200,20}, sprites[1].rotation, 0, 720);
        slider("kampas kojos: $", font_Size , {100, 100, 200,20}, sprites[2].rotation, 0, 720);
        slider("kampas kunelis: $", font_Size , {100, 140, 200,20}, sprites[3].rotation, 0, 720);
        slider("kampas galva: $", font_Size , {100, 180, 200,20}, sprites[4].rotation, 0, 720);
        slider("kampas rankos: $", font_Size , {100, 220, 200,20}, sprites[5].rotation, 0, 720);

        placeRelative();

        for(int i = 0; i < spriteNumb; i++)
            render(i);

        if(evt.type == SDL_QUIT)
            break;
        SDL_RenderPresent(rend);
    }
    return 0;
}
    //d_ausis k_ausis kojos kunelis neutralus_galva rankos
    //  0       1       2      3       4              5

void clearInfo(){
    info.clear();
    info.resize(spriteNumb);
}

void capture(){
    for(int i = 0; i < info.size(); i++){
        info[i].push_back({sprites[i].rotation, sprites[2].x-sprites[i].x, sprites[2].y - sprites[i].y, 10});
    }
}

void save(){
    std::ofstream output{"./assets/animations/stovintis.txt"};
    for(int i = 0; i < info.size(); i++){
        output << info[i].size() << " " <<  i <<" ";

        for(int x = 0; x < info[i].size(); x++)
            output << info[i][x].rotation << " " << info[i][x].relative_x << " " << info[i][x].relative_y << " " << info[i][x].dividend << "   ";

        output << "\n";
    }
};

void render(int i){
    SDL_Rect tmpRect = {sprites[i].x, sprites[i].y, sprites[i].w/10, sprites[i].h/10};
    SDL_RenderCopyEx(rend, sprites[i].txtr, NULL, &tmpRect, sprites[i].rotation, NULL, SDL_FLIP_NONE);
}

void selected0(){ currentSelected = 0;}
void selected1(){ currentSelected = 1;}
void selected2(){ currentSelected = 2;}
void selected3(){ currentSelected = 3;}
void selected4(){ currentSelected = 4;}
void selected5(){ currentSelected = 5;}

void loadSprites(){
    SDL_Surface *tmpSurfc;

    tmpSurfc = SDL_LoadBMP("./assets/images/stovintis/d_ausis.bmp");
    sprites.push_back({360, -100, -100, tmpSurfc->w, tmpSurfc->h, SDL_CreateTextureFromSurface(rend, tmpSurfc)});
    SDL_FreeSurface(tmpSurfc);

    tmpSurfc = SDL_LoadBMP("./assets/images/stovintis/k_ausis.bmp");
    sprites.push_back({360, -100, -100, tmpSurfc->w, tmpSurfc->h, SDL_CreateTextureFromSurface(rend, tmpSurfc)});
    SDL_FreeSurface(tmpSurfc);

    tmpSurfc = SDL_LoadBMP("./assets/images/stovintis/kojos.bmp");
    sprites.push_back({360, -100, -100, tmpSurfc->w, tmpSurfc->h, SDL_CreateTextureFromSurface(rend, tmpSurfc)});
    SDL_FreeSurface(tmpSurfc);

    tmpSurfc = SDL_LoadBMP("./assets/images/stovintis/kunelis.bmp");
    sprites.push_back({360, -100, -100, tmpSurfc->w, tmpSurfc->h, SDL_CreateTextureFromSurface(rend, tmpSurfc)});
    SDL_FreeSurface(tmpSurfc);

    tmpSurfc = SDL_LoadBMP("./assets/images/stovintis/neutralus_galva.bmp");
    sprites.push_back({360, -100, -100, tmpSurfc->w, tmpSurfc->h, SDL_CreateTextureFromSurface(rend, tmpSurfc)});
    SDL_FreeSurface(tmpSurfc);

    tmpSurfc = SDL_LoadBMP("./assets/images/stovintis/rankos.bmp");
    sprites.push_back({360, -100, -100, tmpSurfc->w, tmpSurfc->h, SDL_CreateTextureFromSurface(rend, tmpSurfc)});
    SDL_FreeSurface(tmpSurfc);
    
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

void placeRelative(){
    if(currentSelected == -1 || pressedAButton) return;

    int place_mx, place_my;
    if(SDL_GetMouseState(&place_mx, &place_my) & SDL_BUTTON_LMASK){
        sprites[currentSelected].x = place_mx;
        sprites[currentSelected].y = place_my;
    }

    return;
}

/**
 * Display text. Considers *rend as an existing global variable;
 * @param sentence the text to display.
 * @param textBox the SDL_Rect area where the text should be.
 * @param fontSize is the size of the font.
*/
void renderText(std::string sentence, SDL_Rect textBox, int fontSize){
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

void button(std::string label, SDL_Rect textBox, SDL_Rect buttonBox, void (*onClick)()){
    int button_mx, button_my;
    SDL_GetMouseState(&button_mx, &button_my);

    SDL_RenderDrawRect(rend, &buttonBox);
    renderText(label, textBox, font_Size);

    if(button_mx >= buttonBox.x && button_mx < buttonBox.x + buttonBox.w && button_my >= buttonBox.y && button_my < buttonBox.y + buttonBox.h && evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT){
        pressedAButton = true;
        onClick();
    }    
    return;
}

/**
 * Creats a slider inside SDL_Rect;
 * Assumes that `*rend` and `evt` exists as global variables.
 * In `label` variable, symbol `$` will be replaced with `value`.
 * Uses `renderText` function.
*/
void slider(std::string label, int fontSize, SDL_Rect sliderBox, int &value, int minValue, int maxValue){
    int slider_mx, slider_my;
    int notch_width = sliderBox.h/2, notch_height = sliderBox.h/2;

    SDL_RenderDrawLine(rend, sliderBox.x, sliderBox.y + sliderBox.h/2, sliderBox.x + sliderBox.w - 1, sliderBox.y + sliderBox.h/2);

    if(SDL_GetMouseState(&slider_mx, &slider_my) & SDL_BUTTON_LMASK && slider_mx >= sliderBox.x && slider_mx < sliderBox.x + sliderBox.w && slider_my >= sliderBox.y && slider_my < sliderBox.y + sliderBox.h){
        value = int((double(slider_mx-sliderBox.x)/(sliderBox.w-notch_width))*(maxValue-minValue));
        if(value < minValue) value = minValue;
        if(value > maxValue) value = maxValue;

        pressedAButton = true;
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
    
    SDL_Rect slider_notch = {int(double(double(value)/double(maxValue-minValue))*(sliderBox.w-notch_width)+sliderBox.x), sliderBox.y+sliderBox.h/2-notch_height/2+1, notch_width, notch_height};
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