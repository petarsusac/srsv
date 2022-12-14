1. Upute za pokretanje

Izvršna datoteka može se izgraditi iz izvornog koda pomoću alata CMake. Ako već nije instaliran, alat se može dohvatiti naredbom:

$ sudo apt install cmake

Nakon što je repozitorij kloniran na računalo, potrebno se pozicionirati u poddirektorij lab2, generirati potrebne Makefile-ove za izgradnju programa i na kraju pokrenuti izgradnju. To se može učiniti pokretanjem sljedećih naredbi:

$ cd srsv/lab2/
$ cmake -S . -B build
$ cmake --build build

Izvršna datoteka naziva je lab2, a smještena je u direktorij build. Program se pokreće naredbom:

$ ./build/lab2

Program će se izvršavati i ispisivati status izvođenja sve dok se ne prekine signalom SIGINT, odnosno pritiskom kombinacije tipki Ctrl+C na tipkovnici. Na kraju se ispisuje statistika (po zadacima i ukupno). Nakon ispisa statistike moguće je da će program čekati još neko vrijeme dok se ne probude i završe sve pokrenute dretve, nakon čega program završava s izvođenjem.

Raspored izvođenja zadataka napravljen je ručno (kao u primjeru 4.21 u skripti) i opisan je u komentarima datoteke main.c.

Struktura datoteka:
├── build
│   ...
│   ├── lab2
│   └── Makefile
├── CMakeLists.txt
├── readme.txt
├── inc
│   ├── call.h - makro CALL za lakše pozivanje i ispis grešaka funkcija
│   ├── controller.h
│   ├── input.h
│   ├── periodic_signal.h
│   ├── scheduling.h
│   ├── simulator.h
│   ├── stats.h
│   └── time_utils.h
├── controller.c - logika za upravljanje zadacima, uključujući prekidnu funkciju
├── input.c - klasa čije su instance ulazi na koje dretve zapisuju podatke
├── main.c - kreiranje zadataka i pokretanje sustava
├── periodic_signal.c - sučelje za lakše stvaranje periodičkih signala
├── scheduling.c - logika za određivanje koji zadatak treba obraditi
├── simulator.c - funkcije za dretve simulatore ulaza
├── stats.c - ispis statistike
└── time_utils.c - sučelje za lakši rad s vremenskim sučeljem OS-a (nanosleep i slično), omogućuje da aplikacija koristi milisekunde za računanje vremena umjesto struktura timespec



