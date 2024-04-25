#include"includes.h"
//scans the screen for "platforms"
int WIDTH = GetSystemMetrics(SM_CXSCREEN), HEIGHT = GetSystemMetrics(SM_CYSCREEN);
int UPS = 144;
SDL_Color COLOR_TO_IGNORE = {255,0,255,255};//bright magenta
COLORREF key;
int red, green, blue;
SDL_Color INACCESSIBLE{0,0,0,255};
SDL_Color PLATFORM{0, 255, 0, 255};
SDL_Color CHECKING{255, 0, 230, 255};
SDL_Color SEARCHINGAREA{255,255,255,255};
//std::vector<point> POINTS;
struct pos_struct{
    int x;
    int y;
};
struct cluster_struct{
    std::vector<pos_struct> VECpos;
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
    //no need
    DeleteObject(hbitmap);
    //without 2 delete and 1 release memory increases +~30mb/s until it crashes

    // for(int i = 0; i < WIDTH*HEIGHT*4; i+=4){
    //     r = (int)bitPointer[0];
    //     g = (int)bitPointer[1];
    //     b = (int)bitPointer[2]; //3rd is the alfa channel but its, in this case atleast, is always at 255
    // }

    return;
}

void DISPLAY_SCAN(SDL_Renderer*rend, int x, int y, SDL_Color clr){
    SDL_SetRenderDrawColor(rend, clr.r, clr.g, clr.b, clr.a);
    SDL_RenderDrawPoint(rend, x,y);
    return;
}

//h, scan height intervals;
//colorME, difference in color to be considered as one
//minLength, min length of a platform
std::vector<cluster_struct> cluster;
std::vector<int> states;

bool eightNeighbors(int INDEX, int &colorME, int&r, int&g, int&b, bool pirmas){
    int y = HEIGHT- INDEX/(WIDTH*4);
    int x = INDEX%(WIDTH*4)/4;
    bool bent_Vienas = false;
    int arrIndx = INDEX/4;
    int maxarrSize = WIDTH*HEIGHT;

    if(arrIndx - 1 >= 0) if(states[arrIndx-1] == 0) 
        if(std::abs((int)bitPointer[INDEX-4] - r) <= colorME && std::abs((int)bitPointer[INDEX-3] - r) <= colorME && std::abs((int)bitPointer[INDEX-2] - r) <= colorME){
            bent_Vienas = true;
            states[arrIndx-1] = 1;
            cluster[cluster.size()-1].VECpos.push_back({x-1, y});
            eightNeighbors(INDEX - 4, colorME, r, g, b, false);
        }
    if(arrIndx + 1 < maxarrSize) if(states[arrIndx+1] == 0)
        if(std::abs((int)bitPointer[INDEX+4] - r) <= colorME && std::abs((int)bitPointer[INDEX+5] - r) <= colorME && std::abs((int)bitPointer[INDEX+6] - r) <= colorME){
            bent_Vienas = true;
            states[arrIndx+1] = 1;
            cluster[cluster.size()-1].VECpos.push_back({x+1, y});
            eightNeighbors(INDEX + 4, colorME, r, g, b, false);
        }
    if(arrIndx - WIDTH >= 0) if(states[arrIndx - WIDTH] == 0)
        if(std::abs((int)bitPointer[INDEX-WIDTH*4] - r) <= colorME && std::abs((int)bitPointer[INDEX-WIDTH*4] - r) <= colorME && std::abs((int)bitPointer[INDEX-WIDTH*4] - r) <= colorME){
            bent_Vienas = true;
            states[arrIndx-WIDTH] = 1;
            cluster[cluster.size()-1].VECpos.push_back({x, y-1});
            eightNeighbors(INDEX - WIDTH*4, colorME, r, g, b, false);
        }
    if(arrIndx - WIDTH - 1 >= 0) if(states[arrIndx - WIDTH - 1] == 0)
        if(std::abs((int)bitPointer[INDEX-WIDTH*4 - 4] - r) <= colorME && std::abs((int)bitPointer[INDEX-WIDTH*4 - 4] - r) <= colorME && std::abs((int)bitPointer[INDEX-WIDTH*4 - 4] - r) <= colorME){
            bent_Vienas = true;
            states[arrIndx-WIDTH - 1] = 1;
            cluster[cluster.size()-1].VECpos.push_back({x-1, y-1});
            eightNeighbors(INDEX - WIDTH*4 -4, colorME, r, g, b, false);
        }
    if(arrIndx - WIDTH + 1 >= 0) if(states[arrIndx - WIDTH + 1] == 0)
        if(std::abs((int)bitPointer[INDEX-WIDTH*4 + 4] - r) <= colorME && std::abs((int)bitPointer[INDEX-WIDTH*4 + 4] - r) <= colorME && std::abs((int)bitPointer[INDEX-WIDTH*4+4] - r) <= colorME){
            bent_Vienas = true;
            states[arrIndx-WIDTH + 1] = 1;
            cluster[cluster.size()-1].VECpos.push_back({x+1, y-1});
            eightNeighbors(INDEX - WIDTH*4 + 4, colorME, r, g, b, false);
        }
    if(arrIndx + WIDTH < maxarrSize) if(states[arrIndx + WIDTH] == 0)
        if(std::abs((int)bitPointer[INDEX+WIDTH*4] - r) <= colorME && std::abs((int)bitPointer[INDEX+WIDTH*4] - r) <= colorME && std::abs((int)bitPointer[INDEX+WIDTH*4] - r) <= colorME){
            bent_Vienas = true;
            states[arrIndx+WIDTH] = 1;
            cluster[cluster.size()-1].VECpos.push_back({x, y+1});
            eightNeighbors(INDEX + WIDTH*4, colorME, r, g, b, false);
        }
    if(arrIndx + WIDTH + 1 < maxarrSize) if(states[arrIndx + WIDTH + 1] == 0)
        if(std::abs((int)bitPointer[INDEX+WIDTH*4 + 4] - r) <= colorME && std::abs((int)bitPointer[INDEX+WIDTH*4 + 4] - r) <= colorME && std::abs((int)bitPointer[INDEX+WIDTH*4 + 4] - r) <= colorME){
            bent_Vienas = true;
            states[arrIndx+WIDTH + 1] = 1;
            cluster[cluster.size()-1].VECpos.push_back({x+1, y+1});
            eightNeighbors(INDEX +WIDTH*4 + 4, colorME, r, g, b, false);
        }
    if(arrIndx + WIDTH - 1 < maxarrSize) if(states[arrIndx + WIDTH - 1] == 0)
        if(std::abs((int)bitPointer[INDEX+WIDTH*4-4] - r) <= colorME && std::abs((int)bitPointer[INDEX+WIDTH*4 - 4] - r) <= colorME && std::abs((int)bitPointer[INDEX+WIDTH*4 - 4] - r) <= colorME){
            bent_Vienas = true;
            states[arrIndx+WIDTH - 1] = 1;
            cluster[cluster.size()-1].VECpos.push_back({x-1, y+1});
            eightNeighbors(INDEX +WIDTH*4-4, colorME, r, g, b, false);
        }
    
    states[arrIndx] = 1;
    if(bent_Vienas && pirmas) cluster[cluster.size()-1].VECpos.push_back({x, y});

    return bent_Vienas;
}

void scan(int h, int colorME, int minLength){
    int y, x;
    cluster.clear();
    states.clear();
    for(int i = 0; i < WIDTH*HEIGHT; i++) states.push_back(0);

    for(int index = WIDTH*4*(h); index < 4 * WIDTH * HEIGHT; index += 4){
        y = HEIGHT- index/(WIDTH*4);
        x = index%(WIDTH*4)/4;

        if(states[index/4] == 0){
            cluster.push_back({});
            if(!eightNeighbors(index, colorME, (int&)bitPointer[index], (int&)bitPointer[index+1], (int&)bitPointer[index+2], true))
                cluster.pop_back();
            else{
                if(cluster[cluster.size()-1].VECpos.size() < minLength) cluster.pop_back();
            }
        }
    }
    std::cout <<"ugh";
    return;
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow("floors.cpp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,WIDTH, HEIGHT, 0);
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event windowEvent;

    //Get window handle;
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hWnd = wmInfo.info.win.window;

    MakeWindowTranparent(hWnd);

    while(true){
        if(SDL_PollEvent(&windowEvent)){
            if(SDL_QUIT == windowEvent.type || windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_q)
            break;
        }
        SDL_SetRenderDrawColor(rend, COLOR_TO_IGNORE.r, COLOR_TO_IGNORE.g, COLOR_TO_IGNORE.b, COLOR_TO_IGNORE.a);
        SDL_RenderClear(rend);
        SDL_RenderPresent(rend);

        getScreenPixelInfo(hWnd);
        scan(5,51, 10);//breaks when h<5

        SDL_SetRenderDrawColor(rend, 255,255,255,255);
        for(int x = 0; x <20; x++){
            for(int i = 0; i < cluster[x].VECpos.size(); i++){
                SDL_RenderDrawPoint(rend, cluster[x].VECpos[i].x, cluster[x].VECpos[i].y);
        }
        }

        SDL_RenderPresent(rend);

        SDL_Delay(100); //100ups
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
