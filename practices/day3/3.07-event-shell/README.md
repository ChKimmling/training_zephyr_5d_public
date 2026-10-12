# 3.07 – Status- und Bedienkommandos

## Lernziel

Eine hierarchische Shell-Schnittstelle implementieren, die ausschließlich über ein
öffentliches Service-API Status abfragt und Statistiken bedient.

## Ausgangslage und Aufgabe

Das Projekt enthält Shell-Kommandos `event status`, `event stats` und `event reset`,
eine Service-API sowie kommentierte TODOs in `start/`. Ergänze Service-Snapshots,
Argumentprüfung und Reset-Semantik. Die Shell-Schicht darf keine internen Servicevariablen
direkt lesen.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.07-event-shell/start \
  -d build/day3-3.07
./build/day3-3.07/zephyr/zephyr.exe
```

Für die Referenz verwende `solution/` und `build/day3-3.07-solution`.

## Abnahmesequenz

```text
event help
event status
event stats
event reset
event stats
event reset extra
event stats
```

Zusätzliche Argumente müssen ohne Zustandsänderung abgewiesen werden. Der Status liefert
einen konsistenten Snapshot; der Reset setzt nur Statistikfelder zurück. Beobachte, wie
Shell-Rückgabewert und Fehlermeldung einen Fehler sichtbar machen.

## Erfolgskriterien

- `event`-Unterkommandos erscheinen in Hilfe und Autovervollständigung.
- Shell-Handler enthalten nur Parsing, Darstellung und API-Aufrufe.
- Status, Statistiken und Reset funktionieren ohne Zugriff auf globale Service-Daten.
- Ungültige Argumente werden durch die Shell abgelehnt.
