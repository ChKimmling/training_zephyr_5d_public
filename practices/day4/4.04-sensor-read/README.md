# 4.04 – Emulierten Sensor erfassen und prüfen

## Lernziel

Einen gekapselten Sensorzugriff implementieren, Rückgabewerte behandeln und ausschließlich vollständig gelesene Achswerte an den Aufrufer übergeben.

## Aufgabe

`start/` enthält Overlay, BMI160-Emulator, Stimulushelfer und die öffentliche
Schnittstelle `sensor_source.h`. Ergänze die TODOs in `src/sensor_source.c`.
Der unveränderte Startstand meldet einen unvollständigen Zugriff mit einem Fehlercode.
Die Sensorquelle bleibt unabhängig von Queue und Netzwerk.

| TODO | Datei/Funktion | Auftrag | Nachweis in `solution` |
|---|---|---|---|
| `TODO(4.04-01)` | `sensor_source_init()` | Gerät beziehen und Bereitschaft prüfen | erfolgreicher Start bzw. `-ENODEV` |
| `TODO(4.04-02)` | `sensor_source_read()` | Sample Fetch prüfen | Fehler wird zurückgegeben |
| `TODO(4.04-03)` | `sensor_source_read()` | XYZ-Channel lesen | drei Achsen vorhanden |
| `TODO(4.04-04)` | `sensor_source_read()` | nach mm/s² konvertieren | positive und negative Werte |
| `TODO(4.04-05)` | `sensor_source_read()` | Ausgabe nur vollständig übernehmen | lokale Zwischenstruktur |

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.04-sensor-read/start -d build/day4-4.04-start
./build/day4-4.04-start/zephyr/zephyr.exe

west build -p always -b native_sim \
  training-zephyr/practices/day4/4.04-sensor-read/solution -d build/day4-4.04-solution
./build/day4-4.04-solution/zephyr/zephyr.exe
```

## Abnahmesequenz

1. TODOs im Startstand implementieren und den einzelnen vorbereiteten Stimulus lesen.
2. Für die weitere Prüfung die drei Stimuli aus `solution/src/main.c` verwenden:
   `(0, 0, 9807)`, `(1250, -750, 9500)` und `(-2000, 500, 9000)` in mm/s².
3. Soll- und Istwerte vergleichen und kleine Quantisierungsabweichungen begründen.
4. Mit `sensor_source_fail_next_read(true)` einen einmaligen `-EIO` injizieren.
5. Prüfen, dass die Ausgabeparameter bei einem Lesefehler unverändert bleiben.

Die Referenz zeigt alle drei Stimuli und prüft den injizierten Fehlercode. Sie meldet
`SENSOR_READ PASS`; die Achswerte werden protokolliert, nicht automatisch gegen eine
Toleranz geprüft. Im Startstand ist diese zusätzliche Abnahmesequenz noch zu ergänzen.

## Erfolgskriterien

- Gerätebereitschaft wird vor dem ersten Zugriff geprüft.
- Fetch und Channel Get werden in dieser Reihenfolge ausgeführt und ihre Fehler weitergegeben.
- Positive und negative Achswerte werden in mm/s² konvertiert.
- Fehler erzeugen keine teilweise aktualisierte Ausgabe.
- Wiederholtes Lesen nach dem einmaligen Fehler funktioniert wieder.

## Hinweise und typische Fehler

- XYZ benötigt drei `sensor_value`-Elemente, nicht nur eines.
- Konvertierung erst nach erfolgreichem Channel Get ausführen.
- Den Fehlerhelfer vom tatsächlichen Bus-/Treiberfehler unterscheiden: Er liefert `-EIO`
  bereits vor dem Fetch-Aufruf.

Vertiefung: [Sensor API](../../../exkurs/sensor-api.md).
