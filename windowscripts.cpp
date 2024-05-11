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

HDC hdc, hdcMemory;
DWORD purposeIsToRemoveWarning;
BITMAPINFO bitmap;
bool altLMBPress = false;
bool altRMBPress = false;
bool textInputReady = false;

/**
 * Gets various information about the screen bitmap and saves it to an array ("bitpointer");
*/
void getScreenPixelInfo(){
    //need to make sure we dont create duplicates
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
 * Nupiešia ekrane langą, į kurį galima rašyti. 
*/
void textInput(){

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
        if(std::abs(textRect.w) >= 10 && std::abs(textRect.h) >= 10)
            textInputReady = true;
    }
    return;
}

/**
 * A function that is called when a text input box is active and reads keyboard input.
*/
SDL_TextInputEvent text;
void textInputFunctionallity(){
    SDL_SetRenderDrawColor(rend, 227, 161, 75, 255);
    SDL_RenderDrawRect(rend, &textRect);
    text.timestamp = SDL_GetTicks();
    std::cout << (int)evt.text.text << " "; //the hell?
    //https://wiki.libsdl.org/SDL2/SDL_TextEditingEvent
    //https://wiki.libsdl.org/SDL2/SDL_TextInputEvent

    return;
}

//use this only for debug(or curiosity), but it wont work with scanning thread at the same time!
// void displayFloors(){
//     SDL_SetRenderDrawColor(rend, COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b, COLOR_TO_IGNORE.a);
//     SDL_RenderClear(rend);
//     SDL_RenderPresent(rend);
//     platformPoints = GETSCREENGROUND(platformScanColorME);
//     SDL_SetRenderDrawColor(rend, 255,255,0,255);
//     for(int i = 0; i < platformPoints.size(); i++){
//         SDL_RenderDrawPoint(rend, platformPoints[i].x, platformPoints[i].y);
//     }
// }

/*stupidly slow
    COLORREF key;
    Get device context - in this case, whole display
    HDC hdc = GetDC(NULL);    
    key = GetPixel(hdc, 0, 0);
    red = GetRValue(key);
    green = GetGValue(key);
    blue = GetBValue(key);
*/