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
 *  Gets screen-relative pixel color information and, based on sharp color rgb value change +- color Margin of Error.
 *  @param colorME defines the color Margin of Error.
*/
std::vector<pixel_struct> GETSCREENGROUND(int colorME){
    getScreenPixelInfo();
    std::vector<pixel_struct> cluster{};
    int y, x;
    for(int index = WIDTH*4; index < 4 * WIDTH * HEIGHT; index += 4){
        y = HEIGHT- index/(WIDTH*4);
        x = index%(WIDTH*4)/4;

        if(std::abs((int)bitPointer[index] - (int)bitPointer[index-WIDTH*4]) > colorME && std::abs((int)bitPointer[index+1] - (int)bitPointer[index-WIDTH*4+1]) > colorME && std::abs((int)bitPointer[index+2] - (int)bitPointer[index-WIDTH*4+2]) > colorME)
            cluster.push_back({x,y});
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




//use this only for debug(or curiosity), but do not use it with threads at the same time!
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
