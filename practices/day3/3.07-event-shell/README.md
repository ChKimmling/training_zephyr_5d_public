# 3.07 – Status- und Bedienkommandos

## Lernziel

Eine hierarchische Shell-Schnittstelle implementieren, die ausschließlich über ein
öffentliches Service-API Status abfragt und Statistiken bedient.

## Ausgangslage und Aufgabe

Das Projekt enthält Shell-Kommandos `event status`, `event stats` und `event reset`, eine Service-API sowie kommentierte TODOs in
`start/`. Ergänze mutex-geschützte Service-Snapshots und die dokumentierte Reset-Semantik.
Die Shell-Schicht darf keine internen Servicevariablen direkt lesen. Die Shell-Registrierung
legt für jedes Kommando die genaue Argumentzahl fest; zusätzliche Argumente werden daher
bereits von der Shell abgewiesen, bevor der Handler ausgeführt wird.

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

Beide Versionen verwenden dieselbe Shell-Konfiguration und denselben Anwendungseinstieg. In
`start/` sind die Service-Aufgaben als TODOs markiert; `solution/` implementiert sie.

## Abnahmesequenz

```text
event help
event
event status
event stats
event reset
event stats
event reset extra
event stats
event status extra
event unknown
```

`event status` zeigt den Initialisierungszustand, Gerätebereitschaft, Eingangspegel,
Stimuluszustand und letzten Fehler. `event stats` zeigt `raw`, `processed` und `rejected`.
Am Anfang sind alle Zähler null. `event reset` setzt ausschließlich diese drei Zähler zurück;
Status und Konfiguration bleiben unverändert.

`event reset extra` und `event status extra` müssen die falsche Argumentzahl melden, ohne den
Handler aufzurufen oder Zähler zu ändern. `event unknown` muss von der Shell als unbekanntes
Unterkommando abgewiesen werden; `event` ohne Unterkommando zeigt eine Hilfeübersicht. Eine
ungültige Shell-Eingabe erhöht `rejected_count` nicht:
dieser Zähler ist für vom Service abgewiesene fachliche Event-Anforderungen reserviert.
`event stats` vor der Initialisierung liefert den definierten Fehler `-EAGAIN` statt eines
ungültigen Zugriffs oder Absturzes; die Shell übersetzt ihn in eine verständliche Meldung.

## Erfolgskriterien

- `event`-Unterkommandos erscheinen in Hilfe und Autovervollständigung.
- Shell-Handler enthalten nur Parsing, Darstellung und API-Aufrufe.
- Status, Statistiken und Reset funktionieren ohne Zugriff auf globale Service-Daten.
- Zu viele Argumente und unbekannte Unterkommandos werden durch die Shell abgelehnt.
- Fehler des Service werden mit Fehlermeldung und Fehlerstatus an die Shell zurückgegeben.
- Der Reset löscht nur Zähler; unzulässige Shell-Aufrufe verändern keine Zähler.
- Logging bleibt für Diagnose reserviert; Bedienergebnisse erscheinen in der Shell.
