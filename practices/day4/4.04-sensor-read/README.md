# 4.04 – Emulierten Sensor erfassen und prüfen

## Lernziel und Ausgangslage

Implementiere den kleinsten vollständigen Sensorzugriff. Overlay, BMI160-Emulator und
Stimulushelfer sind vorbereitet. Bearbeitungszeit: etwa 25 Minuten.

| TODO | Datei/Funktion | Auftrag | Nachweis in `solution` |
|---|---|---|---|
| `TODO(4.04-01)` | `sensor_source_init()` | Gerät beziehen und Bereitschaft prüfen | erfolgreicher Start bzw. `-ENODEV` |
| `TODO(4.04-02)` | `sensor_source_read()` | Sample Fetch prüfen | Fehler wird zurückgegeben |
| `TODO(4.04-03)` | `sensor_source_read()` | XYZ-Channel lesen | drei Achsen vorhanden |
| `TODO(4.04-04)` | `sensor_source_read()` | nach mm/s² konvertieren | positive und negative Werte |
| `TODO(4.04-05)` | `sensor_source_read()` | Ausgabe nur vollständig übernehmen | lokale Zwischenstruktur |

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.04-sensor-read/start -d build/day4-4.04-start
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.04-sensor-read/solution -d build/day4-4.04-solution
./build/day4-4.04-solution/zephyr/zephyr.exe
```

Abnahme: drei Stimuli werden plausibel gelesen, ein injiziertes `-EIO` wird nicht als
Messwert ausgegeben und `SENSOR_READ PASS` erscheint.

