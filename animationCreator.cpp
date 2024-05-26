#include<SDL2/SDL.h>

#include<ft2build.h>
#include FT_FREETYPE_H

#include<iostream>
#include<vector>

int WIDTH = 1000;
int HEIGHT = 600;

FT_Library ft;
FT_FaceRec_* face;
int font_Size = 12;
SDL_Color colors[256];

SDL_Renderer* rend;

struct{
    SDL_Rect tmp = {20,20, WIDTH-20, font_Size};

}txtBoxes;

void loadFonts(int fontSize);
void renderText(std::string sentence, SDL_Rect &textBox, int fontMaxHeight);

int main(int argc, char *argv[]){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)) std::cout << "Failed to initialize SDL!\n";
    if(FT_Init_FreeType(&ft)) std::cout << "Failed to initialize FreeType library!\n";
    loadFonts(font_Size);

    SDL_Window* wind = SDL_CreateWindow("animator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    rend = SDL_CreateRenderer(wind, -1, 0);
    SDL_Event evt;

    while(true)
    {
        SDL_PollEvent(&evt);
        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);

        //Possibly a flag to put everything in a single line?
        //renderText("Heyo daiud huwahd hawuihd ahwuidh iawdu hawidh uih", txtBoxes.tmp, font_Size);
        //slider

        if(evt.type == SDL_QUIT)
            break;
        SDL_RenderPresent(rend);
    }
    return 0;
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

// /**
//  * 
// */
// void slider(SDL_Renderer* renderer, SDL_Event* event, int ID, std::string label, int label_rect_ID, double* value, double start_value, double end_value, bool* moving, int x, int y, int w, int h){
//     //Draw the bar
//     for(int i = 0; i < h; i++)
//         SDL_RenderDrawLine(renderer, x, y + i, x + w, y + i);
//     //Notch/button thing
//     if(onButton(ID))
//         if(event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT){
//             xStart = MOUSEx;
//             *moving = true;
//         }
//     if(event->type == SDL_MOUSEMOTION && *moving){
//         xEnd = MOUSEx;
//         *value = (xEnd - xStart) * (end_value -start_value) / w + *value;
//         xStart = MOUSEx;
//     }
//     //on button up, unclick all buttons
//     if(event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
//         *moving = false;

//     rects[ID].x = x + (*value - start_value )/(end_value - start_value) * w;
//     if(rects[ID].x < x){
//         rects[ID].x = x;
//         *value = start_value;
//     }
//     else if(rects[ID].x > x + w){
//         rects[ID].x = x + w;
//         *value = end_value;
//     }

//     //Label text
//     SDL_FreeSurface(tsurface);
//     tsurface = TTF_RenderUTF8_Blended(font, (label.c_str()), {255,255,255,255});
//     SDL_DestroyTexture(ttexture);
//     ttexture = SDL_CreateTextureFromSurface(renderer, tsurface);
//     SDL_RenderCopy(renderer, ttexture, nullptr, &rects[label_rect_ID]);
//     SDL_RenderDrawRect(renderer, &rects[ID]);
//     return;
// }