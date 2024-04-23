Informatikos projektinis darbas naudojant SDL2 ir c++.

Kompiuterio "draugas" - žmogeliukas bėgiojantis po kompiuterį.

> Elina ir Alanas

Kaip paruošti visus failus:

1) Atsisiųsti [SDL_template.rar](https://mega.nz/file/C4s0gTba#i0FvQPJ17krVxF2zDfwVAS7QkJHXMqUdDVPUh6A5pIc)
2) Atsisiųsti [MSYS2](https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe), instaliuoti (path | kelio nekeisti). Atsisiųsti [mingw32](https://gist.github.com/charitra1022/dbc6de742452bff3680782d343e5289b). Tuomet ieškoti kompiuteryje "Edit the system environment variables" -> "Environment Variables" -> 'Variable' skiltyje pačiam viršuje rasti "Path" -> Paspausti ir "edit" -> "new" -> įrašyti "C:\msys64\mingw32".
3) Atsisiųsti [Visual Studio Code](https://code.visualstudio.com/)
4) Atidaryti vscode ir atsisiųsti extension [GitHub Pull Requests](https://marketplace.visualstudio.com/items?itemName=GitHub.vscode-pull-request-github) (nepamirškite kitų reikalingų extensions)
5) Pačiame vscode paspausti ant "Accounts" kairėje apačioje virš nustatymų mygtukų ir prisijungti prie github "Sign in with GitHub to use GitHub pull requests".
6) Kairėje apačioje spausti "Open a Remote Window" ir "Open Remote Repository", kuriame, po "* GitHub" nerodo jokio vardo.
7) Kai norite pradėti rašyti **naują** kodą, apačioje kairėje antras mygtukas po "GitHub" su 'main' užrašu paspauskit, pažymėkite "Create new branch", pavadinkite savo vardu ir išmestoje lentelėje spauskit "switch to branch".
8) Atidarykite terminalą (ctrl + j) ir įrašykite:
   > git clone https://github.com/AlanMatiusevskij/Kurybinis_Darbas.git
9) Jums turi išsisaugoti aplankas vietoje "C:\Users\jūsų_vardas\Kurybinis_Darbas" (patariu išsisaugoti C:\Users\jūsų_vardas shorcut'ą (ne pačio kurybinio_darbo aplanko!). Kairėje viršuje spauskite "file", "open folder" ir "show local".
   Atidarykite Kurybinis_Darbas aplanką. Ten 'extractinkite' SDL_template.rar.
10) Rašote kodą, jeigu norite, sukurkite .cpp ir .h failus ir t.t.. Kai norite testuoti, į terminalą (ctrl + j) rašykite:
    > mingw32-make
    > 
    > ./main
    >
    > //make komanda sukuria programą .exe, o ./main ją tiesiog atidaro
11) Kai kodas veikia ir norite jį sinchronizuoti su projektu, mygtukų juostoje kairėje spauskite "Source Control" ir "Commit & Push". Patikrinkite ar nėra nesutapimų, gal reikalingas kodas ištrinamas ir gal yra klaidų su kito žmogaus atnaujintu pagrindiniu kodu. Jeigu viskas gerai, patvirtinkite "Pull request" ir ištrinkite "Kurybinis_darbas" aplanką savo kompiuteryje.
12) kartokite punktą nr 6.
