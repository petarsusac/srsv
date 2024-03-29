1. Upute za pokretanje

Izvršna datoteka može se izgraditi iz izvornog koda pomoću alata CMake. Ako već nije instaliran, alat se može dohvatiti naredbom:

$ sudo apt install cmake

Nakon što je repozitorij kloniran na računalo, potrebno se pozicionirati u direktorij lab1, generirati potrebne Makefile-ove za izgradnju programa i na kraju pokrenuti izgradnju. To se može učiniti pokretanjem sljedećih naredbi:

$ cd srsv/lab1/
$ cmake -S . -B build
$ cmake --build build

Izvršna datoteka naziva je lab1, a smještena je u direktorij build. Program se pokreće uz prosljeđivanje parametra K kao argumenta. K mora biti cijeli broj od 1 do 5. Npr. pokretanje uz K=2:

$ build/lab1 2

Ako se ne zada parametar K, program će se pokrenuti s pretpostavljenom vrijednosti K (K=2).

Program će se izvršavati i ispisivati status izvođenja sve dok se ne prekine signalom SIGINT, odnosno pritiskom kombinacije tipki Ctrl+C na tipkovnici. Nakon što je poslan signal za prekid simulacije, potrebno je pričekati još nekoliko sekundi dok ne završe sve pokrenute dretve koje eventualno spavaju u trenutku prekida. Tada se ispisuje statistika (za svaki ulaz i na kraju ukupna) i program završava s izvođenjem.



2. Usporedba rezultata izvođenja uz različite vrijednosti K

Za ispitivanje programa korištene su fiksne postavke ulaza uz promjenu parametra K. Program je ručno prekinut nakon 30-ak sekundi rada.

Postavke ulaza su sljedeće:

Ukupan broj ulaza: 20
Period: 500 ms, broj ulaza: 7
Period: 1000 ms, broj ulaza: 6
Period: 2000 ms, broj ulaza: 4
Period: 5000 ms, broj ulaza: 3

Uz takve postavke i K=2, postignuta je rubna ispravnost sustava (~3% zakašnjelih odgovora). Jedini ulazi koji su primili kasne odgovore su oni s najmanjim periodom promjene (500 ms). Oni su primili između 0 i 10% zakašnjelih odgovora.

Ukupna statistika:

Broj promjena stanja ulaza: 451
Prosjecno vrijeme odgovora: 169.63 ms
Maksimalno vrijeme odgovora: 751 ms
Broj zakasnjelih odgovora: 14 (3.10%)


Uz K=1 broj zakašnjelih odgovora je značajno porastao, što je i očekivano s obzirom da su zbog K=1 dretve simulatori generirale ulaze svaki period. Zbog toga je upravljačka petlja imala puno podataka za obraditi u svakoj iteraciji, a s obzirom da obrada traje određeno vrijeme nije stigla odgovoriti na svaku promjenu ulaza na vrijeme. Ulazi s najkraćom periodom su primili čak i do 90% kasnih odgovora, što i nije iznenađujuće s obzirom da je prosječno vrijeme odgovora bilo dulje od njihovog perioda.

Ukupna statistika:

Broj promjena stanja ulaza: 517
Prosjecno vrijeme odgovora: 568.41 ms
Maksimalno vrijeme odgovora: 1364 ms
Broj zakasnjelih odgovora: 232 (44.87%)

Uz K=5 nije bilo zakašnjelih odgovora. Opet, takav rezultat je očekivan jer sada dretve simulatori nisu generirale ulaze svaki period, već je moguće da su čekale čak i do 4 perioda prije generiranja ulazne vrijednosti, pa upravljačka petlja nije morala dugo obrađivati podatke u svakoj iteraciji. Prosječno vrijeme odgovora bilo je kraće od 100 ms.

Ukupna statistika:

Broj promjena stanja ulaza: 210
Prosjecno vrijeme odgovora: 81.69 ms
Maksimalno vrijeme odgovora: 305 ms
Broj zakasnjelih odgovora: 0 (0.00%)

