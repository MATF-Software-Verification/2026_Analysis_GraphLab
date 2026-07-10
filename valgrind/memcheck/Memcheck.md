# Valgrind Memcheck

Ovaj direktorijum sadrži rezultate pokretanja Valgrind Memcheck alata nad QtTest unit test executable-om `graphlab_unit_tests`.

## Pokretanje

```bash
cd valgrind/memcheck
./run_memcheck.sh
```

Skripta podrazumeva Qt 6.8.0 na putanji `/home/kalu/programs/qt/6.8.0/gcc_64`. Ako je Qt instaliran drugde:

```bash
QT_PREFIX=/putanja/do/Qt/6.x/gcc_64 ./run_memcheck.sh
```

## Šta skripta radi

Skripta:

- konfiguriše poseban Debug build unit testova bez coverage instrumentacije;
- prevodi `graphlab_unit_tests`;
- pokreće executable pod Valgrind Memcheck alatom;
- čuva pun izlaz u `memcheck-unit-tests.txt`;
- izdvaja kratak pregled u `memcheck-summary.txt`.

Analizom su obuhvaceni model grafa, algoritmi i serijalizacija koji se izvršavaju kroz unit testove. GUI deo aplikacije nije obuhvaćen ovim Memcheck pokretanjem.

## Rezultati

- `memcheck-unit-tests.txt`: pun izlaz build-a, QtTest-a i Valgrind Memcheck-a.
- `memcheck-summary.txt`: kratak pregled Valgrind rezultata.

Dobijeni rezultat:

```text
26 passed, 0 failed
definitely lost: 0 bytes in 0 blocks
indirectly lost: 0 bytes in 0 blocks
possibly lost: 0 bytes in 0 blocks
ERROR SUMMARY: 0 errors from 0 contexts
```

Memcheck prijavljuje `still reachable` memoriju iz Qt/GLib runtime-a. To nije klasifikovano kao memory leak (`definitely lost`, `indirectly lost` i `possibly lost` su 0).
