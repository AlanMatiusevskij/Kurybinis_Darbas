#include"includes.h"
//scans the screen for "platforms"
int WIDTH = GetSystemMetrics(SM_CXSCREEN), HEIGHT = GetSystemMetrics(SM_CYSCREEN);
int UPS = 144;
SDL_Color COLOR_TO_IGNORE = {255,0,255,255};//bright magenta
int red, green, blue;
struct pos_struct{
    int x;
    int y;
};
struct cluster_struct{
    pos_struct pos;
};
bool MakeWindowTranparent(HWND hWnd){
    COLORREF colorKey = RGB(COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b);

    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

    return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}
BYTE *bitPointer = NULL;
HDC hdc, hdcMemory;
DWORD REMOVEWARNING;
BITMAPINFO bitmap;
void getScreenPixelInfo(HWND hWnd){
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
    HBITMAP hbitmap = CreateDIBSection(hdcMemory, &bitmap, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, REMOVEWARNING);
    SelectObject(hdcMemory, hbitmap);
    BitBlt(hdcMemory, 0, 0, WIDTH, HEIGHT, hdc, 0, 0, SRCCOPY);
    DeleteObject(hbitmap);
    //without 2 of 'delete' and 1 of 'release' memory increases +~30mb/s until it crashes

    return;
}

std::vector<cluster_struct> scan(int colorME){
    std::vector<cluster_struct> cluster;
    int y, x;
    cluster.clear();
    for(int index = WIDTH*4; index < 4 * WIDTH * HEIGHT; index += 4){
        y = HEIGHT- index/(WIDTH*4);
        x = index%(WIDTH*4)/4;

        if(std::abs((int)bitPointer[index] - (int)bitPointer[index-WIDTH*4]) > colorME && std::abs((int)bitPointer[index+1] - (int)bitPointer[index-WIDTH*4+1]) > colorME && std::abs((int)bitPointer[index+2] - (int)bitPointer[index-WIDTH*4+2]) > colorME){
            cluster.push_back({x,y});
        }
    }
    return;
}

// int main(int argc, char *argv[]){
//     SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
//     SDL_Window* window = SDL_CreateWindow("floors.cpp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIDTH, HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP);
//     SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
//     SDL_Event windowEvent;
//     //Get window handle;
//     SDL_SysWMinfo wmInfo;
//     SDL_VERSION(&wmInfo.version);
//     SDL_GetWindowWMInfo(window, &wmInfo);
//     HWND hWnd = wmInfo.info.win.window;

//     MakeWindowTranparent(hWnd);

//     while(true){
//         if(SDL_PollEvent(&windowEvent)){
//             if(SDL_QUIT == windowEvent.type || windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_q)
//             break;
//         }
//         SDL_SetRenderDrawColor(rend, COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b, COLOR_TO_IGNORE.a);
//         SDL_RenderClear(rend);
//         SDL_RenderPresent(rend);

//         getScreenPixelInfo(hWnd);
//         scan(10);

//         SDL_SetRenderDrawColor(rend, 255,255,0,255);
//         for(int i = 0 ; i < cluster.size(); i++)
//            SDL_RenderDrawPoint(rend, cluster[i].pos.x, cluster[i].pos.y);

//         SDL_RenderPresent(rend);

//         SDL_Delay(100); //10ups
//     }
//     SDL_Quit();
//     return EXIT_SUCCESS;
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
//window's display zoom sugadina*