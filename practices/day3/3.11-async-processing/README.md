# 3.11 – Input Events asynchron verarbeiten

## Lernziel

`event_input_chan` mit einem zbus Async Listener konsumieren und Empfang von fachlicher
Verarbeitung unterscheiden. Die Callback-Arbeit läuft außerhalb des GPIO- und Shell-Kontexts.

## Aufgabe

Vervollständige in `start/` die TODOs in `src/event_listener.c`:

1. Registriere einen Async Listener am Input-Channel.
2. Prüfe den Channel und Message-Pointer.
3. Aktualisiere `input_received` und `processed` erst an den passenden Stellen.
4. Logge Quelle, Sequenznummer und Ergebnis.
5. Prüfe, dass Shell-/Producer-Arbeit nicht auf die Verarbeitung warten muss.

Die Beispielanwendung publiziert fünf Test-Events. Ein erfolgreich abgeschlossener Durchlauf
endet mit `input_published = input_received = processed = 5`.

## Build

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.11-async-processing/start \
  -d build/day3-3.11
./build/day3-3.11/zephyr/zephyr.exe
```

## Erfolgskriterien

- Input Events werden genau einmal empfangen und verarbeitet.
- Der Async Listener hält die Publikation nicht durch synchrone Facharbeit auf.
- Fehler oder ungültige Nachrichten werden nicht als `processed` gezählt.
- Zählerbedeutungen sind von Command-Zählern getrennt.
