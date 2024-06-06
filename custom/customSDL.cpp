/**
 * Custom functions and classes that are useful while creating SDL applications.
*/
#include "customSDL.h"

//////////////////////
//Functions to check things associated with SDL_Rects.

bool customsdl::onRect(SDL_Rect rect){
    int x, y;
    SDL_GetMouseState(&x, &y);
    return onRect(rect, x, y);
}
bool customsdl::onRect(SDL_Rect rect, int x, int y){
    if(rect.x <= x && rect.x + rect.w > x && rect.y <= y && rect.y + rect.h > y) return true;
    return false;
}
bool customsdl::checkIfRectsEqual(SDL_Rect rect1, SDL_Rect rect2){
    if(rect1.x == rect2.x && rect1.w == rect2.w && rect1.h == rect2.h && rect1.y == rect2.y) return true;
    return false;
}

/////////////////
//Defining surfaceManipulation class's functions.

void customsdl::surfaceManipulation::createSurface(int width, int height, int depth, SDL_PixelFormatEnum format){
    surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format);
    SDL_SetSurfaceBlendMode(surface, SDL_BlendMode::SDL_BLENDMODE_ADD);    
    SDL_LockSurface(surface);
}
void customsdl::surfaceManipulation::drawToSurface(__pixel coords, __color RGBA){
    if(RGBA.a == 0) return;
    data = (Uint8*)surface->pixels + coords.y*surface->pitch + coords.x * surface->format->BytesPerPixel;
    *((Uint32*)data) = (Uint32)(RGBA.r << 0 | RGBA.g << 8 | RGBA.b << 16 | RGBA.a << 24);
}
SDL_Texture* customsdl::surfaceManipulation::createTextureAndDeleteSurface(SDL_Renderer *renderer){
    SDL_Texture* _return;
    SDL_UnlockSurface(surface);
    _return = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return _return;
}
customsdl::__color customsdl::surfaceManipulation::getSurfaceColors(__pixel coords, SDL_Surface *surface, bool freeSurface){
    Uint8 red{}, green{}, blue{}, alfa{};
    Uint32 values = *(Uint32*)((Uint8*)((Uint8*)surface->pixels + coords.y*surface->pitch + coords.x*surface->format->BytesPerPixel));
    SDL_GetRGBA(values, surface->format, (Uint8*)(&red), (Uint8*)(&green), (Uint8*)(&blue), (Uint8*)(&alfa));
    if(freeSurface) SDL_FreeSurface(surface);
    return {red, green, blue, alfa};
}
void customsdl::surfaceManipulation::blitSurface(SDL_Rect areaTO, SDL_Surface* from){
    if(surface == nullptr){
        std::cout << "surfaceManipulation class: SURFACE WAS NOT CREATED!\n";
        return;
    }
    SDL_UnlockSurface(surface);
    SDL_BlitSurface(from, NULL, surface, &areaTO);
    SDL_LockSurface(surface);
}
void customsdl::surfaceManipulation::blitSurface(SDL_Rect areaTO, SDL_Surface* from, SDL_Rect areaFROM){
    if(surface == nullptr){
        std::cout << "surfaceManipulation class: SURFACE WAS NOT CREATED!\n";
        return;
    }
    SDL_UnlockSurface(surface);
    SDL_BlitSurface(from, &areaFROM, surface, &areaTO);
    SDL_LockSurface(surface);
}

/////////////////////////////
//UI class's function declarations.

void customsdl::UI::renderText(SDL_Renderer* renderer, SDL_Texture *text, SDL_Rect textToRenderArea){
    destination = &lastTextInfo.dimensions;
    SDL_RenderCopy(renderer, text, &textToRenderArea, destination);
}
void customsdl::UI::renderText(SDL_Renderer* renderer, SDL_Texture *text){
    destination = &lastTextInfo.dimensions;
    SDL_RenderCopy(renderer, text, NULL, destination);
}
FT_FaceRec* customsdl::UI::useFont(std::string path, int fontsize){
    //Check if a specific sized font is already loaded.
    for(_activeFaces_struct &obj : __activeFaces)
        if(obj.path == path && obj.fontSize == fontsize) return obj.face;
    
    //Otherwise, load a font of specific size.
    __activeFaces.push_back({path, fontsize});
    int i = __activeFaces.size()-1;

    FT_Open_Args args;
    args.flags = FT_OPEN_PATHNAME;
    args.pathname = path.data();

    FT_Init_FreeType(&__activeFaces[i].ft);
    if(FT_Open_Face(__activeFaces[i].ft, &args, 0, &__activeFaces[i].face)) std::cout << "Failed to load font ('" << path << "') of size " << fontsize << ". Tried index: " << i <<"\n";
    
    __activeFaces[i].face->glyph->format = FT_GLYPH_FORMAT_BITMAP;
    FT_Set_Pixel_Sizes(__activeFaces[i].face, fontsize, fontsize);

    return __activeFaces[i].face;
}
SDL_Texture* customsdl::UI::findExistingTextTexture(std::string &sentence, SDL_Rect &textBox){
    for(_activeTextFields_struct &obj : __activeTexts){
        if(obj.sentence == sentence && checkIfRectsEqual(textBox, obj.dimensions)){
            obj.lastAccessed = 0;
            return obj.texture;
        }
    }
    return nullptr;
}
SDL_Texture* customsdl::UI::textFieldTexture(SDL_Renderer *renderer, std::string sentence, SDL_Rect textBox, int fontsize, std::string fontpath, bool autoNewLines){
    SDL_Texture* textr = findExistingTextTexture(sentence, textBox);
    if(textr != nullptr) return textr;
    SDL_DestroyTexture(textr);

    std::vector<std::string> words;
    std::string ind_word{""};

    FT_FaceRec_* FACE = useFont(fontpath, fontsize);

    //Save each word and whitespaces in a vector
    for(char symb : sentence){
        if(symb == ' '){
            words.push_back(ind_word);
            ind_word = "";
        }
        else ind_word += symb;
    }
    //save the last one.
    words.push_back(ind_word);

    //Find the total width and height.
    int cWidth = 1;
    int cHeight = 1;
    int wordWidth = 0;
    int maxWidth = 0;
    int maxHeight = 0;

    for(std::string word : words){
        for(char symb : word){
            FT_Load_Char(FACE, symb, FT_LOAD_BITMAP_METRICS_ONLY);
            cWidth+=FACE->glyph->bitmap.width;
            wordWidth+=FACE->glyph->bitmap.width;

            if(autoNewLines && cWidth >= textBox.w || symb == '\n'){
                cHeight+=fontsize+1;
                cWidth-=wordWidth;
                maxWidth = std::max(maxWidth, cWidth);

                cWidth = wordWidth;
            }
        }
        wordLength.push_back(wordWidth);
        wordWidth = 0;
        cWidth+=fontsize/4;
        maxWidth = std::max(maxWidth, cWidth);
    }
    maxHeight = cHeight+fontsize+4;

    //Create a surface where the sentence will be stored.
    surfaceManipulation manip;
    manip.createSurface(maxWidth, maxHeight, 32, SDL_PIXELFORMAT_RGBA32);
    lastTextInfo.dimensions = {textBox.x, textBox.y, maxWidth, maxHeight};
    lastTextInfo.fontSize = fontsize;

    //Load each glyph surface and merge them into a single surface.
    int totalGlyphWidth = 1, totalWordWidth = 1;
    int totalHeight = 1;

    int belowBaseLine{};

    for(int i = 0; i < words.size(); i++){
        if(autoNewLines && i != 0 && totalWordWidth+wordLength[i] >= textBox.w){
            totalHeight+=fontsize+1;
            totalWordWidth = 1;
            totalGlyphWidth = 1;
        }
        for(char symb : words[i]){
            FT_Load_Char(FACE, symb, FT_LOAD_RENDER);
            belowBaseLine = (FACE->glyph->metrics.height - FACE->glyph->metrics.horiBearingY)/55;
            SDL_Surface *glyph;

            if(autoNewLines && totalGlyphWidth >= textBox.w || symb == '\n'){
                totalHeight+=fontsize+1;
                totalGlyphWidth = 1;
                totalWordWidth = 1;
            }
            
            glyph = surf8bitTo32bit(SDL_CreateRGBSurfaceFrom(FACE->glyph->bitmap.buffer, FACE->glyph->bitmap.width, FACE->glyph->bitmap.rows, 8, FACE->glyph->bitmap.pitch, 0, 0, 0, 0xFF));
            manip.blitSurface({totalGlyphWidth, totalHeight-glyph->h+fontsize-fontsize/5+belowBaseLine, 0, 0}, glyph);
            SDL_FreeSurface(glyph);

            //Update some info.
            totalGlyphWidth+=FACE->glyph->bitmap.width;
            totalWordWidth+=FACE->glyph->bitmap.width;
            //textInfo.widthSymEnd.push_back({totalGlyphWidth, totalHeight});
        }
        totalWordWidth+=fontsize/4;
        totalGlyphWidth+=fontsize/4;
    }
    
    //Create a texture and store it.
    __activeTexts.push_back({manip.createTextureAndDeleteSurface(renderer), sentence, 0, textBox});
    return __activeTexts[__activeTexts.size()-1].texture;
}
SDL_Surface* customsdl::UI::surf8bitTo32bit(SDL_Surface* _8bit){
    SDL_SetPaletteColors(_8bit->format->palette, _8bitpalleteColors, 0, 256);
    SDL_SetSurfaceBlendMode(_8bit, SDL_BlendMode::SDL_BLENDMODE_ADD); 
    SDL_Surface* _return = SDL_ConvertSurfaceFormat(_8bit, SDL_PIXELFORMAT_RGBA32, 0);

    SDL_FreeSurface(_8bit);
    return _return;
}