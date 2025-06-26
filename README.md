# Bentorpedo

## Áttekintés
A klasszikus torpedó játék terminálban, C nyelven, ncurses felülettel és OpenMP-vel gyorsított speciális támadásokkal. A játék MI ellen történik, több nehézségi szinttel, köztük adaptívval.

## Főbb funkciók
- Pályaméret és hajók számának beállítása (egyéni játék)
- MI ellenfél (könnyű, közepes, nehéz, adaptív)
- Speciális támadások és védekezések (OpenMP gyorsítással)
- ncurses alapú, teljesen magyar nyelvű, retro menürendszer
- Saját hajók elhelyezése (egyéni módban manuális, klasszikus módban automatikus)
- Statisztikák mentése és megjelenítése nehézségi szintenként
- Játék közbeni információk: eltelt idő, körök száma, speciális akció visszatöltődés
- Két tábla (saját és ellenfél) középre igazítva, díszítő kerettel
- Játék közbeni súgó, minden vezérlés magyarul
- Speciális támadás/védekezés csak feltételekkel használható
- Hibás lépésnél, tiltott akciónál magyar hibaüzenet
- Mindenhol egységesen 'H' karakterrel jelölt hajók

## Projekt felépítése
```
torpedo
├── src
│   ├── main.c        # Felhasználói felület, menük, főciklus, statisztika
│   ├── game.c        # Játék logika (táblák, MI, speciális akciók)
│   ├── game.h        # Fejlécek, struktúrák, konstansok
├── Makefile          # Fordítási utasítások
└── README.md         # Dokumentáció
```

## Fordítás
A projekt könyvtárában:
```
make (Linux)
```
A fordítás után csak a végső bináris marad meg, az object fájlok törlődnek.

## Futtatás
```
./Bentorpedo
```

## Függőségek
- ncurses
- OpenMP

## Játék közbeni vezérlés

- Nyilak: kurzor mozgatása (lövés vagy hajóelhelyezés)
- SPACE / ENTER: lövés a kiválasztott mezőre
- a: speciális támadás (csak feltételekkel)
- d: speciális védekezés (csak feltételekkel)
- q: játék megszakítása
- F12: rejtett DEBUG mód ki-/bekapcsolása (lásd lent)

## Játék menete

- Klasszikus játék: 10x10-es pálya, 5 hajó, közepes MI, hajók automatikusan elhelyezve
- Egyéni játék: minden paraméter állítható, hajók elhelyezése manuális
- Statisztikák: minden nehézségi szinthez külön top 10 lista, csak győzelem esetén kerül be az eredmény
- A játék közben a két tábla a képernyő két oldalán, középen az aktuális információk láthatók
- A játékos csak olyan mezőre lőhet, amit még nem lőtt meg
- Speciális támadás/védekezés csak akkor használható, ha legalább egy hajót kilőttél, és az akció nincs visszatöltődés alatt

## Rejtett DEBUG mód

A játékban elérhető egy rejtett DEBUG mód, amely játék közben a képernyő bal alsó részén folyamatosan megjeleníti a játékos minden akcióját (kurzor mozgás, lövés, speciális akciók stb.).
- Aktiválás/kikapcsolás: F12 billentyűvel játék közben.
- A legutóbbi 8 akció látható a keret alsó részén.
- A súgóban nem jelenik meg, csak fejlesztői célra ajánlott.
