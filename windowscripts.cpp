#include"includes.h"
/*
    About:
        A file that holds to window, screen and drawing related functions.
    Bugs:
        window's display zoom sugadina pixel location ( check when drawing points and increasing system display scale value);
*/

//Defining variables

/** bitPointer is a pointer to an array of pixel rgb values.
*   Each pixel is indexed every 4 elements as each element stores a particular value: | red, green, blue, alpha |*/
BYTE *bitPointer = NULL;

int typeIndex = 0;
HDC hdc, hdcMemory;
DWORD purposeIsToRemoveWarning;
BITMAPINFO bitmap;
bool altLMBPress = false;
bool altRMBPress = false;
bool textInputReady = false;
bool allowedToType = true;

std::vector<std::string> words_str;
char* answer = (char*)"-";

/**
 * Gets various information about the screen bitmap and saves it to an array ("bitpointer");
*/
void getScreenPixelInfo(){
    DeleteDC(hdcMemory);
    ReleaseDC(hWnd, hdc); 
    hdc = GetDC(HWND_DESKTOP);
    hdcMemory = CreateCompatibleDC(hdc);

    bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
    bitmap.bmiHeader.biWidth = WIDTH;
    bitmap.bmiHeader.biHeight = HEIGHT;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    bitmap.bmiHeader.biCompression = BI_RGB;
    bitmap.bmiHeader.biSizeImage = WIDTH * 4 * HEIGHT;
    bitmap.bmiHeader.biClrUsed = 0;
    bitmap.bmiHeader.biClrImportant = 0;
    HBITMAP hbitmap = CreateDIBSection(hdcMemory, &bitmap, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, purposeIsToRemoveWarning);
    SelectObject(hdcMemory, hbitmap);
    BitBlt(hdcMemory, 0, 0, WIDTH, HEIGHT, hdc, 0, 0, SRCCOPY);

    DeleteObject(hbitmap);
    
    //without 2 of 'delete' and 1 of 'release' memory increases +~30mb/s until program crashes
    return;
}

/**
 *  Gets screen-relative pixel color information and, based on sharp color rgb value change +- color Margin of Error.
 *  @param colorME defines the color Margin of Error.
*/
std::vector<int> GETSCREENGROUND(int colorME){
    getScreenPixelInfo();
    std::vector<int> cluster{};
    cluster.resize(WIDTH*HEIGHT);
    for(int index = WIDTH*4*8; index < 4 * WIDTH * HEIGHT; index += 4){
        if(std::abs((int)bitPointer[index] - (int)bitPointer[index-WIDTH*4]) > colorME && std::abs((int)bitPointer[index+1] - (int)bitPointer[index-WIDTH*4+1]) > colorME && std::abs((int)bitPointer[index+2] - (int)bitPointer[index-WIDTH*4+2]) > colorME)
            cluster[WIDTH*HEIGHT - index/4 - 1] = 1;
    }
    return cluster;
}

/**
 * Padaro, kad ekranas ignoruotų tam tikrą spalvą.
*/
bool MakeWindowTranparent(){
    COLORREF colorKey = RGB(COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b);
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

// ne 0 - pakeltas
// 0 - nuspaustas
void altFunc(){
    //just a circle?
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    SDL_RenderDrawPoint(rend, mx, my); SDL_RenderDrawPoint(rend, mx + 1, my);SDL_RenderDrawPoint(rend, mx-1, my);
    SDL_RenderDrawPoint(rend, mx, my + 1); SDL_RenderDrawPoint(rend, mx, my -1);

    if(GetAsyncKeyState(VK_LBUTTON) == 0 && altLMBPress) altLMBPress = false;
    if(GetAsyncKeyState(VK_LBUTTON) != 0 && !altLMBPress){
        altLMBPress = true;
        togoPoints.clear();
        togoPoints.push_back({mx, -1});
    }

    if(GetAsyncKeyState(VK_RBUTTON) != 0 && !altRMBPress) textRect = {mx, my, 0, 0};
    if(GetAsyncKeyState(VK_RBUTTON) != 0){
        textInputReady = false;
        textRect = {textRect.x, textRect.y, mx - textRect.x, my - textRect.y};
        altRMBPress = true;
        SDL_RenderDrawRect(rend, &textRect);
    }
    else{
        altRMBPress = false;
        if(std::abs(textRect.w) >= 10 && std::abs(textRect.h) >= 10){
            textInputReady = true;
            textinput = "";
            typeIndex = 0;
        }
    }
    return;
}

/**
 * A function that is called when a text input box is active and reads keyboard input.
 * todo: get window focus. for some reason doesnt react when pressing the upper part of the body??
*/
void textInputFunctionallity(){
    SDL_SetRenderDrawColor(rend, 227, 161, 75, 255);
    SDL_RenderDrawRect(rend, &textRect);

    //Gauti tekstą/užklausą.
    if(evt.type == SDL_TEXTINPUT && allowedToType){
        textinput.insert(typeIndex, std::string(evt.text.text));
        typeIndex++;
    }
    if(evt.type == SDL_KEYDOWN && allowedToType){
        switch(evt.key.keysym.sym){
            case SDLK_BACKSPACE:
                if(textinput.size() > 0){
                    textinput.erase(textinput.begin() + typeIndex-1);
                    typeIndex--;
                }
                break;
            case SDLK_ESCAPE:
                textInputReady = false;
                break;
            case SDLK_LEFT:
                typeIndex--;
                break;
            case SDLK_RIGHT:
                typeIndex++;
                break;
            case SDLK_RETURN:
                chatGPTinquiry(words_str);
                break;
        }
        textinput.shrink_to_fit();
        if(typeIndex > textinput.size()) typeIndex = textinput.size()-1;
        if(typeIndex < 0) typeIndex = 0;
    }

    displayText(textinput, textRect, fontSize-fontSize/5);
    return;
}

/**
 * Loads the main .ttf type font.
*/
void loadFonts(){
    FT_Open_Args args;
    args.flags = FT_OPEN_PATHNAME;
    char fontpath[] = "./assets/fonts/OpenSans-Regular.ttf";
    args.pathname = fontpath; 
    if(FT_Open_Face(ft, &args, 0, &face)) std::cout << "Failed to load a font!\n";
    face->glyph->format = FT_GLYPH_FORMAT_BITMAP;
    FT_Set_Pixel_Sizes(face, fontSize, fontSize);

    for(int i = 0; i < 256; i++)
        colors[i].r = colors[i].g = colors[i].b = colors[i].a= i;
    return;
}

/**
 * Display text.
 * @param sentence the text to display.
 * @param textBox the SDL_Rect area where the text should be.
 * @param fontMaxHeight is the maximum height of the font.
*/
void displayText(std::string sentence, SDL_Rect &textBox, int fontMaxHeight){
    words_str.clear();
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
            SDL_Rect pos = {textBox.x + totalWidth, int(textBox.y + y + fontMaxHeight - glyph->h + belowBaseLine), glyph->w, glyph->h};
            
            //Check if the letter goes to a new line.
            totalWidth+=glyph->w;
            if(totalWidth>=textBox.w){
                y += 24;
                totalWidth = 1 + glyph->w;
                pos.x = textBox.x + 1;
                pos.y = int(textBox.y + y + fontMaxHeight - glyph->h + belowBaseLine);
            }

            //Display the letter.
            SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, glyph);
            SDL_Rect renderArea = {0, 0, std::min(textBox.w-totalWidth, glyph->w), std::min(textBox.h-y, glyph->h)};
            SDL_RenderCopy(rend, texture, &renderArea, &textBox);
            
            //Clean up.
            SDL_FreeSurface(glyph);
            SDL_DestroyTexture(texture);
        }
        totalWidth+=6;
        letterCount = 0;
    }
    return;
}

/**
 * Starts the python application to send the contents of the text field to chatgpt (python applicaiton)
*/
void chatGPTinquiry(std::vector<std::string> words){
    std::string allText = "";
    for(int i = 0; i < words.size(); i++)
        allText += words[i];

    allowedToType = false;
    pyclink::communicate("./chatbot.exe", allText.c_str(), &answer, 5000, true);

    return;
}

/**
 * A function which waits for chatgpt's response.
*/
void updateResponse(){
    if(answer != "-"){
        allowedToType = true;
        textinput.clear();
        for(int i = 0; i < strlen(answer); i++)
            textinput += answer[i];
        answer = (char*)"-";
    }
    return;
}

/*stupidly slow
    COLORREF key;
    Get device context - in this case, whole display
    HDC hdc = GetDC(NULL);    
    key = GetPixel(hdc, 0, 0);
    red = GetRValue(key);
    green = GetGValue(key);
    blue = GetBValue(key);
*/