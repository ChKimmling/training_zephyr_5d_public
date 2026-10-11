# 1.10 – Praxis 3: Eigene minimale Anwendung

## Ziel

Eine kleine, eigenständige Zephyr-Anwendung aus CMake-, Konfigurations- und C-Dateien aufbauen.

## Ausgangssituation

`start/` enthält nur ein minimales Gerüst. Die vollständige Zustandslogik ist in `solution/`
als Referenz enthalten.

## Aufgabe

Erstelle beziehungsweise vervollständige:

1. `CMakeLists.txt`, die Zephyr findet und `src/main.c` einbindet;
2. `prj.conf` mit der für Konsolenausgabe nötigen Konfiguration;
3. `src/main.c` mit den Zuständen `STARTUP`, `ACTIVE` und `LIMIT_REACHED`;
4. einen periodischen Zähler, der beim Grenzwert den Zustand ändert und ausgibt.

## Vorgehen

```sh
west build -p always -b native_sim training-zephyr/practices/day1/1.10-minimal-application/start \
  -d build/day1-1.10
./build/day1-1.10/zephyr/zephyr.exe
```

Führe den Build nach jeder Etappe erneut aus. Das Gerüst in `start/` ist bewusst lauffähig,
aber erfüllt noch nicht die vollständige Aufgabenstellung.

## Erwartetes Ergebnis

Die Lösung meldet `Device starting`, danach die Zählerwerte 1 bis 5. Ab Wert 5 erscheint der
Zustand `LIMIT_REACHED`. Der Prozess bleibt zur Beobachtung aktiv.
