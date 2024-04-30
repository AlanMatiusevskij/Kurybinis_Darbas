Informatikos projektinis darbas naudojant SDL2 ir c++.

Kompiuterio "draugas" - žmogeliukas bėgiojantis po kompiuterį.

> Elina ir Alanas

Kaip paruošti visus failus:

1) Atsisiųsti [SDL_template.rar](https://mega.nz/file/C4s0gTba#i0FvQPJ17krVxF2zDfwVAS7QkJHXMqUdDVPUh6A5pIc)
2) Atsisiųsti [MSYS2](https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe), instaliuoti (path | kelio nekeisti). Atsisiųsti [mingw32](https://gist.github.com/charitra1022/dbc6de742452bff3680782d343e5289b). Tuomet ieškoti kompiuteryje "Edit the system environment variables" -> "Environment Variables" -> 'Variable' skiltyje pačiam viršuje rasti "Path" -> Paspausti ir "edit" -> "new" -> įrašyti "C:\msys64\mingw32".
3) Atsisiųsti [Visual Studio Code](https://code.visualstudio.com/)
4) Atidaryti vscode ir atsisiųsti extension [GitHub Pull Requests](https://marketplace.visualstudio.com/items?itemName=GitHub.vscode-pull-request-github) (nepamirškite kitų reikalingų extensions)
5) Pačiame vscode paspausti ant "Accounts" kairėje apačioje virš nustatymų mygtukų ir prisijungti prie github "Sign in with GitHub to use GitHub pull requests".
6) Nueiti į kažkokį aplanką, kuriame norėtumėte išsaugoti repository aplanką, ir terminale (ctrl + j) parašykite:
   > git clone https://github.com/AlanMatiusevskij/Kurybinis_Darbas.git
7) Padarykite keitimus, kurkite failus, kai norite testuoti, į terminalą (ctrl + j) rašykite:
    > mingw32-make
    > 
    > ./main
    >
    > //make komanda sukuria programą .exe, o ./main ją tiesiog atidaro
8) Kai kodas veikia ir norite jį sinchronizuoti su projektu, mygtukų juostoje kairėje spauskite "Source Control" ir "Commit & Push". Patikrinkite ar nėra nesutapimų, gal reikalingas kodas ištrinamas ir gal yra klaidų su kito žmogaus atnaujintu pagrindiniu kodu. Jeigu viskas gerai, patvirtinkite "Pull request" ir ištrinkite "Kurybinis_darbas" aplanką savo kompiuteryje.
9) Kartokite punktą nr 6.
