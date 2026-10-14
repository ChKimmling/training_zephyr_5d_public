# 4.05 – Portable Probe-Struktur und Messwertbewertung

## Lernziel

Sensordaten in einer transportneutralen `motion_probe` mit Sequenznummer, monotonem Erfassungszeitpunkt und Qualitätsstatus darstellen.

## Aufgabe

Die Sensorquelle aus 4.04 ist vollständig. Bearbeite `src/motion_probe.c`; der
öffentliche Vertrag steht in `include/motion_probe.h`. Einheit ist mm/s², der gültige
Bereich ist einschließlich ±20000 mm/s².

Der Aufrufer liefert Sequenz und Zeit. `motion_probe_build()` übernimmt diese auch bei
Sensorfehlern. Die Probe enthält keine Geräteinstanz und keine Socketdaten.

| TODO | Auftrag | Lösungsnachweis |
|---|---|---|
| `TODO(4.05-01)` | alle Felder deterministisch initialisieren | `memset` vor Bewertung |
| `TODO(4.05-02)` | Sequenz übernehmen | Ausgabe/Test prüft Sequenz |
| `TODO(4.05-03)` | monotone Zeit speichern | Alter basiert auf `k_uptime_get()` |
| `TODO(4.05-04)` | Sensor- und Bereichsqualität setzen | `OK`, `RANGE`, `SENSOR_ERROR` |
| `TODO(4.05-05)` | nicht negatives Datenalter | zukünftiger Zeitpunkt ergibt 0 |

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.05-motion-probe/start -d build/day4-4.05-start
./build/day4-4.05-start/zephyr/zephyr.exe

west build -p always -b native_sim \
  training-zephyr/practices/day4/4.05-motion-probe/solution -d build/day4-4.05-solution
./build/day4-4.05-solution/zephyr/zephyr.exe
```

## Abnahmesequenz

1. Startstand vervollständigen und die vorbereitete Messung ausführen.
2. Sequenz 1, Qualität `OK` und ein nicht negatives Alter erwarten. Die Referenz gibt
   zusätzlich die Achswerte aus; sie besitzt keine eigene PASS-Marke.
3. Mit direkten Eingabedaten genau ±20000 sowie ±20001 für jede Achse prüfen.
4. Einen negativen Sensor-Rückgabewert einspeisen: Qualität `SENSOR_ERROR`, Achsen null,
   Sequenz und Erfassungszeitpunkt bleiben erhalten.
5. Für `captured_ms=100` das Alter bei 90, 100 und 125 prüfen: 0, 0 und 25 ms.

Diese Grenzfälle werden von der einzelnen vorbereiteten Messung nicht abgedeckt.
Die Testübung 4.07 liefert dafür eine erweiterbare Prüfstruktur.

## Erfolgskriterien

- Alle Felder werden deterministisch initialisiert.
- ±20000 bleibt `OK`; Überschreitung einer Achse ergibt `RANGE`.
- Sensorfehler werden nicht als gültige Nullmessung interpretiert.
- Das Datenalter verwendet dieselbe monotone Zeitbasis wie der Erfassungszeitpunkt.
- Der Vertrag bleibt unabhängig von Sensorhardware und Transport.

## Hinweise und typische Fehler

- `STALE`, `GAP` und `OVERLOAD` sind im Enum vorgesehen; die aktuelle Erzeugung setzt
  ausschließlich `OK`, `RANGE` oder `SENSOR_ERROR`.
- Das berechnete Alter setzt nicht automatisch den Qualitätsstatus auf `STALE`.
- Fehler- und Bereichsfälle mit direkten Daten prüfen, damit Sensorbegrenzungen die
  fachlichen Grenztests nicht verfälschen.
