## Apie
Informatikos projektinis darbas - Petto.

Tai 'kompiuterinis gyvūnėlis'. 

Gyvūnėlis ar kitas veikėjas bėgiojantis po ekraną, reaguojantis į įvairius veiksnius bei gebantis komunikuoti su naudotoju.

> Elina ir Alanas

## Programos ir sistemos, reikalingos kompiliuojant projektą

1) Instaliuoti [MSYS2](https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe)
2) Atidaryti **MSYS2 MSYS** ir įrašyti (eilės tvarka, spausti 'enter').
   * `pacman -Syu`, kad atnaujinti paketus.
   * `pacman -Syu`, kad atnaujinti sistemas.
   * `pacman -S --needed base-devel mingw-w64-i686-toolchain` (pasirinkti viską), kad atsisiųsti kompiliavimo sistemą.
3) Surasti **Edit the system environment variables -> Environment variables -> Path -> New** ir įrašyti kelią `C:\msys64\mingw32\bin` (jeigu nepakeitėte MSYS2 kelio).
4) Naudoti pasirinktą IDE, rekomenduoju [Visual Studio Code](https://code.visualstudio.com/).
5) Instaliuoti [git sistemą](https://github.com/git-for-windows/git/releases/download/v2.45.0.windows.1/Git-2.45.0-64-bit.exe).
6) Instaliuoti [python](https://www.python.org/downloads/release/python-3123/) programavimo kalbą.
7) Atidaryti **cmd** kaip administratorius ir įrašyti.
   * `pip install nuitka`, kad instaliuoti nuitka kompiliavimo sistemą.
   
<sub>[SDL_template.rar](https://mega.nz/file/C4s0gTba#i0FvQPJ17krVxF2zDfwVAS7QkJHXMqUdDVPUh6A5pIc)</sub>
## Kaip gauti, atnaujinti 'repositories' ir naudotis git sistema
Komandas rašyti į terminalą. Kai matote `+`, tai toje vietoje įrašykite, ko reikalaujama.

1) `git clone + nuoroda` atsiųs repository.
2) `git branch` parodys egzistuojančias šakas. Nuspalvintai pažymėta šaka yra šiuo metu atidaryta.
3) `git checkout + šakos_pavadinimas` atidarys nurodytą šaką.
4) `git pull` atsiųs ir sujungs pakeitimus su naujesne versija tuo metu atidarytos šakos.
5) `git pull origin + šakos_pavadinimas` atsiųs ir sujungs pakeitimus iš kitos šakos. `origin`, tai repository nuoroda.
6) `git add + pridėto_failo_pavadinimas` pridės failą, kurį .git sistema visada atnaujins ir išsaugos.
7) `git commit -a -m " + labai_trumpas_pakeitimo_aprašymas "` paruoš atnaujintus failus sujungimui ir išsaugojimui.
8) `git push` paruoštus atnaujinimus ir pakeitmus sujungs ir išsaugos repository'je.
10) `git config user.name " + slapyvardis/vardas "`, `git config user.email " + elPaštas "` nustatys, kieno vardu ir el. paštu daromi pakeitimai.
11) `git config (--global) --unset user.email`, `git config (--global) --unset user.name` panaikins įvestą paštą ir slapyvardį.
12) `git status` duos informacijos apie šakos pakeitimus.
13) Jeigu norite atsijungti iš **git** kompiuteryje, reikia surasti **Credential manager -> Windows credentials -> Generic credentials**. Čia surasti **git** ir spausti **remove**.

## Kaip sukurti SDL (c++) programą
1) Atidaryti aplanką, kuriame yra repository, tai yra, jūsų IDE yra atidarytas **tik** tame aplanke.
2) Atidaryti terminalą ir įrašyti.
   * `mingw32-make`, kad sukompiliuoti programą.
     
Viskas, galite rasti ir atidaryti **main.exe** failą arba atidaryti per terminalą `./main`.

## Kaip sukurti python programą
1) Atidaryti jūsų IDE (arba cmd) kaip administratorius.
2) Atidaryti aplanką, kuriame yra repository, tai yra, jūsų IDE yra atidarytas **tik** tame aplanke.
3) Atidaryti terminalą ir įrašyt.
   * `py -m nuitka +pitonoFailoPavadinimas.py --mingw64 --standalone --onefile --disable-console`
4) Galite pašalinti `.build`, `.dist` ir `.onefile-build` aplankus.

Viskas, galite rasti ir atidaryti **pitonoFailoPavadinimas.exe** arba įrašyti `./pitonoFailoPavadinimas` (pavadinimui pakeisti kompiliavimo metu galite naudoti `--product-name` flag'ą)