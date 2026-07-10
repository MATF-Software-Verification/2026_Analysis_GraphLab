# Unit Tests

Ovaj direktorijum sadrzi QtTest unit testove za izdvojenu logiku projekta GraphLab: model grafa, algoritme nad grafovima, genericku logiku iteracija algoritama i serijalizaciju `.graph` fajlova. Testovi se grade iz analysis repozitorijuma nad izvornim fajlovima iz `GraphLab/src`.

## Zavisnosti

Za pokretanje su potrebni:

- Python 3
- CMake
- C++ kompajler sa podrskom za C++17
- Qt 6 sa modulima `Core` i `Test`
- `ctest`
- `lcov` i `genhtml`
- `gcov` ili `gcov-10`

Na Ubuntu sistemu, deo zavisnosti se moze instalirati komandom:

```bash
sudo apt install cmake g++ lcov
```

Qt se koristi iz lokalne Qt instalacije. Podrazumevana putanja je `/home/kalu/programs/qt/6.8.0/gcc_64`, a druga putanja se zadaje promenljivom `QT_PREFIX`.

## Pokretanje

```bash
cd unit_tests
./run_tests.py
```

Skripta podrazumeva Qt 6.8.0 na putanji `/home/kalu/programs/qt/6.8.0/gcc_64`. Ako je Qt instaliran drugde:

```bash
QT_PREFIX=/putanja/do/Qt/6.x/gcc_64 ./run_tests.py
```

## Rezultati

Skripta pokrece CMake konfiguraciju, prevodi testove, izvrsava ih preko CTest/QtTest i generise coverage pomocu `lcov`/`genhtml`.

Poslednji dobijeni rezultat:

```text
26 passed, 0 failed
Line coverage: 93.6%
Function coverage: 85.8%
```

Testovi su podeljeni po oblastima:

- `tests/graph_unit_tests.cpp`: osnovne operacije nad grafom i QVariant konverzija.
- `tests/algorithm_unit_tests.cpp`: algoritmi nad grafovima i iteracije algoritama.
- `tests/serialization_unit_tests.cpp`: cuvanje, ucitavanje i ponasanje pri nepostojecem fajlu.

Rezultati pokretanja nalaze se u:

- `test-results/`: izlazi konfiguracije, build-a i testova.
- `test-results/qtest-output.txt`: QtTest rezultat.
- `coverage/lcov.info`: filtrirani coverage rezultat.
- `coverage/html/index.html`: HTML coverage izvestaj.

HTML coverage izvestaj se otvara u pregledaču:

```bash
xdg-open coverage/html/index.html
```

Coverage se odnosi na izdvojenu ne-GUI logiku iz `GraphLab/src/graph`, `GraphLab/src/algorithm` i `GraphLab/src/Serialization`. GUI fajlovi nisu ukljuceni u ovaj coverage izvestaj.
