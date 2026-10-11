# 2.09 – Praxis 2: Race Condition beheben

## Ziel

Eine Read–Modify–Write-Race-Condition reproduzieren und gemeinsamen Zustand mit einem Mutex
schützen. Eine Semaphore dient separat zur Ereignissignalisierung.

## Ausgangssituation

Zwei Worker erhöhen denselben Zähler. Das Gerüst enthält absichtlich eine Unterbrechung
zwischen Lesen und Schreiben. Der erwartete Endwert ist doppelt so groß wie die Anzahl der
Iterationen pro Worker. Eine Semaphore meldet das Ende der Worker an `main()`.

## Aufgabe

1. Baue und starte das ungeschützte Gerüst mehrfach. Vergleiche erwarteten und tatsächlichen
   Endwert.
2. Markiere gemeinsam veränderte Daten und den vollständigen kritischen Abschnitt.
3. Ergänze einen Mutex. Schütze Lesen, Ändern und Schreiben; halte den Abschnitt so kurz wie
   möglich.
4. Prüfe den Rückgabewert von Mutex-Operationen und stelle sicher, dass die Semaphore auch bei
   einem Fehlerpfad signalisiert wird.
5. Ändere `SIGNAL_COMPLETION` auf `0`, um den endlichen Timeout-Pfad sichtbar zu prüfen.
6. Erkläre, warum der Mutex den Zähler schützt, die Semaphore dagegen das Ereignis signalisiert.

## Vorgehen

```sh
west build -p always -b native_sim training-zephyr/practices/day2/2.09-race-condition-exercise/start \
  -d build/day2-2.09
./build/day2-2.09/zephyr/zephyr.exe
```

Baue nach jeder Änderung erneut. Der Test endet selbstständig. Für die Referenz:

```sh
west build -p always -b native_sim training-zephyr/practices/day2/2.09-race-condition-exercise/solution \
  -d build/day2-2.09-solution
./build/day2-2.09-solution/zephyr/zephyr.exe
```

## Prüfkriterien

- Der geschützte Lauf erreicht wiederholt den erwarteten Endwert.
- Der Timeout-Pfad ist mit `SIGNAL_COMPLETION=0` erreichbar und wird gemeldet.
- Mutex und Semaphore erfüllen getrennte Aufgaben.
- Es gibt keinen Deadlock und der kritische Abschnitt umfasst nur den gemeinsamen Zugriff.

## Verwendete Zephyr-APIs

`K_THREAD_DEFINE`, `k_mutex_lock`, `k_mutex_unlock`, `k_sem_take`, `k_sem_give`, `k_yield`
