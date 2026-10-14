# 4.05 – Portable Probe-Struktur und Qualitätsmodell

Die Sensorquelle aus 4.04 ist vollständig. Ergänzt wird ausschließlich der transportneutrale
Probe-Vertrag. Einheit ist mm/s², der gültige Bereich einschließlich ±20000 mm/s².

| TODO | Auftrag | Lösungsnachweis |
|---|---|---|
| `TODO(4.05-01)` | alle Felder deterministisch initialisieren | `memset` vor Bewertung |
| `TODO(4.05-02)` | Sequenz übernehmen | Ausgabe/Test prüft Sequenz |
| `TODO(4.05-03)` | monotone Zeit speichern | Alter basiert auf `k_uptime_get()` |
| `TODO(4.05-04)` | Sensor- und Bereichsqualität setzen | `OK`, `RANGE`, `SENSOR_ERROR` |
| `TODO(4.05-05)` | nicht negatives Datenalter | zukünftiger Zeitpunkt ergibt 0 |

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.05-motion-probe/start -d build/day4-4.05-start
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.05-motion-probe/solution -d build/day4-4.05-solution
```

Bearbeitungszeit: etwa 30 Minuten. Devkits und Transporte sind nicht beteiligt.

