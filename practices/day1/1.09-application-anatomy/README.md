# 1.09 – Anatomie einer Zephyr-Anwendung

## Ziel

Schnittstelle, Implementierung und Build-Einbindung eines zusätzlichen C-Moduls nachvollziehen.

## Ausgangssituation

Die Anwendung verwendet `include/status.h` und `src/status.c`, um ihren Zustand auszugeben.

## Aufgabe

1. Ordne `main.c`, `status.c`, `status.h`, `CMakeLists.txt` und `prj.conf` ihren Aufgaben zu.
2. Verfolge den Aufruf von `main()` bis `status_report()`.
3. Ergänze ein Modul `src/counter.c` mit passendem Header und Funktion.
4. Trage die Quelldatei in CMake ein und rufe die Funktion aus `main.c` auf.
5. Baue und starte das erweiterte Projekt.

## Vorgehen

```sh
west build -p always -b native_sim training-zephyr/practices/day1/1.09-application-anatomy/start \
  -d build/day1-1.09
./build/day1-1.09/zephyr/zephyr.exe
```

Eine fehlende CMake-Quelldatei kann zu einem Linkerfehler führen. Die vorhandene Anwendung
funktioniert vor deiner Erweiterung bereits.

## Erwartetes Ergebnis

Die Referenzlösung gibt `Status: ACTIVE` und `Counter module: 1` aus.
Nach der Beobachtung den laufenden `native_sim`-Prozess mit Ctrl+C beenden.
