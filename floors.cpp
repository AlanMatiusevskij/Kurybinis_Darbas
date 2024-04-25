#include<iostream>
#include<vector>
#include<SDL2/SDL.h>
#include<chrono>
#include<SDL2/SDL_syswm.h>
#include <Windows.h>

int WIDTH = GetSystemMetrics(SM_CXSCREEN), HEIGHT = GetSystemMetrics(SM_CYSCREEN);
int UPS = 60;
SDL_Color COLOR_TO_IGNORE = {255,0,255,255};//bright magenta
COLORREF key;
int red, green, blue;

struct point{
    int x1, y1, x2, y2;
};
//std::vector<point> POINTS;

bool MakeWindowTranparent(HWND hWnd){
    COLORREF colorKey = RGB(COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b);

    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

void getScreenPixelInfo(HWND hWnd){
    HDC hdc, hdcMemory;
    BYTE *bitPointer;
    DWORD REMOVEWARNING;
    int r, g, b;

    hdc = GetDC(HWND_DESKTOP);
    hdcMemory = CreateCompatibleDC(hdc);

    BITMAPINFO bitmap;
    bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
    bitmap.bmiHeader.biWidth = WIDTH;
    bitmap.bmiHeader.biHeight = HEIGHT;
    bitmap.bmiHeader.biPlanes = 1;
    bitmap.bmiHeader.biBitCount = 32;
    bitmap.bmiHeader.biCompression = BI_RGB;
    bitmap.bmiHeader.biSizeImage = WIDTH * 4 * HEIGHT;
    bitmap.bmiHeader.biClrUsed = 0;
    bitmap.bmiHeader.biClrImportant = 0;
    HBITMAP hbitmap = CreateDIBSection(hdcMemory, &bitmap, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, REMOVEWARNING);
    SelectObject(hdcMemory, hbitmap);
    BitBlt(hdcMemory, 0, 0, WIDTH, HEIGHT, hdc, 0, 0, SRCCOPY);

    for(int i = 0; i < WIDTH*HEIGHT*4; i+=4){
        r = (int)bitPointer[0];
        g = (int)bitPointer[1];
        b = (int)bitPointer[2];
    }

    return;
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow("floors.cpp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIDTH, HEIGHT, 0);
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, 0);
    SDL_Event windowEvent;

    //Get window handle;
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hWnd = wmInfo.info.win.window;

    getScreenPixelInfo(hWnd);
    MakeWindowTranparent(hWnd);

    while(true){
        SDL_SetRenderDrawColor(rend, COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b, COLOR_TO_IGNORE.a);
        SDL_RenderClear(rend);

        if(SDL_PollEvent(&windowEvent)){
            if(SDL_QUIT == windowEvent.type || windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_q)
            break;
        }
        SDL_RenderPresent(rend);
        SDL_Delay((1/float(UPS))*1000);
    }
    SDL_Quit();
    return EXIT_SUCCESS;
}
/*stupidly slow
    Get device context - in this case, whole display
    HDC hdc = GetDC(NULL);    
    key = GetPixel(hdc, 0, 0);
    red = GetRValue(key);
    green = GetGValue(key);
    blue = GetBValue(key);
*/
/* chrono:
    std::chrono::steady_clock::time_point now;
    std::chrono::steady_clock::time_point then;
    now = std::chrono::steady_clock::now();
    then = std::chrono::steady_clock::now();
    std::chrono::duration<double> timespan = std::chrono::duration_cast<std::chrono::seconds>(then-now);
    std::cout << "took to complete: " << timespan.count();
*/
/*SDL thread
    SDL_Thread* thr;
    thr = SDL_CreateThread(iterate, "test", static_cast<void*>(NULL));
*/
