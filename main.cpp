//VISKA KOMENTUOJAM
#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
//mes nenaudojam 'using namespace std', todėl prieš cout ir cin ir kitas komandas reik naudoti std::

int main(int argc, char *argv[]){
    //Sukuriamas langas, renderer'is, ir event'u (kas vyksta ekrana) "saugykla"
    SDL_Window* wind = SDL_CreateWindow("langas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, 0);
    SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event evt;

    while(true){
        //Sakome, kad render'is turi priesti pilka spalva, o SDL_renderClear ekrana isvalo, bet tiksliau viska ta, dabar pilka, spalva isdazo
        SDL_SetRenderDrawColor(rend, 30,30,30,255);
        SDL_RenderClear(rend);
        //gaunami paskutiniai yvikiai ekrane
        SDL_PollEvent(&evt);
        //jeigu yvikis koks nors isjungimas programos, tai isjungiame
        if(evt.type == SDL_QUIT)
            break;
        
        //kiekvienas while() prasisukimas, tai kaip 1 frame'as, kai 60fps, tai 60 kartu prasisuka. Kiekviena karta kai piesiame y renderi, mes piesiame y jo "bufferi"
        //tai zodziu, paruosiame duomenis ant lekstes, o gale reikia tuos duomenis pateikti naudotojui.
        SDL_RenderPresent(rend);
    }

    SDL_Quit();
    return EXIT_SUCCESS; //galima tiesiog 0, cia yra #define dalykas, kury pamokyt galiu, not sure what exactly its purpose is
}
//CHANGED