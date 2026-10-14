# 3.07 – zbus-Steuerkanal

## Lernziel

Schreibende Shell-Kommandos als Nachrichten modellieren, die über `event_cmd_chan` an einen
zbus Message Subscriber übertragen werden.

## Ausgangslage und Aufgabe

Das Projekt enthält Shell-Kommandos `event start`, `event stop` und `event reset`,
`event_command`, den `event_cmd_chan` und ein Subscriber-Gerüst. Ergänze in `start/` die
Command-Erzeugung, Publikation und Subscriber-Verarbeitung. Die Shell darf die Fachlogik nicht
direkt aufrufen.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.07-event-shell/start \
  -d build/day3-3.07
./build/day3-3.07/zephyr/zephyr.exe
```

Für die Referenz verwende `solution/` und ein eigenes Build-Verzeichnis:

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.07-event-shell/solution \
  -d build/day3-3.07-solution
./build/day3-3.07-solution/zephyr/zephyr.exe
```

Beide Versionen verwenden dieselbe Shell-Konfiguration und denselben öffentlichen
Nachrichtenvertrag. In `start/` sind TODOs für Publisher und Subscriber markiert.

## Abnahmesequenz

```text
event help
event stats
event start
event stats
event stop
event start extra
event stats
event reset
```

`event stats` zeigt bereits das gemeinsame Tages-Schema. In dieser Stufe werden
`cmd_published`, `cmd_received` und `cmd_rejected` befüllt; Input-Zähler bleiben null, bis
spätere Übungen den Input-Channel ergänzen. `cmd_published` steigt nur nach erfolgreicher
Publikation. `cmd_received` wird vom Subscriber gezählt; nach Queue-Drain sind beide für
gültige Requests gleich. Falsche Syntax darf nichts publizieren. Der Reset-Command setzt die
vereinbarten Zähler zurück.

## Erfolgskriterien

- `event start/stop/reset` werden über den Command-Channel publiziert.
- Shell-Handler enthalten nur Parsing, Darstellung und Command-Publikation.
- Subscriber empfängt gültige Commands genau einmal.
- Zu viele Argumente und unbekannte Unterkommandos werden durch die Shell abgelehnt.
- Ungültige Shell-Aufrufe publizieren keine Commands.
- Logging bleibt für Diagnose reserviert; Bedienergebnisse erscheinen in der Shell.
