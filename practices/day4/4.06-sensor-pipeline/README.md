# 4.06 – Sensor-Pipeline mit Message Queue

Die Queue besitzt vier Plätze. Die Referenzpolicy lautet „neuester Wert gewinnt“: Bei voller
Queue wird genau der älteste Eintrag entfernt und der Drop einmal gezählt.

| TODO | Auftrag | Lösungsnachweis |
|---|---|---|
| `TODO(4.06-01)` | nicht blockierend einstellen | `k_msgq_put(..., K_NO_WAIT)` |
| `TODO(4.06-02)` | Überlastpolicy umsetzen | sechs Eingaben ergeben zwei Drops |
| `TODO(4.06-03)` | Fehler- und Dropzähler schützen | Snapshot enthält stabile Zähler |
| `TODO(4.06-04)` | Consumer entnimmt vollständige Probe | vier Proben verarbeitet |
| `TODO(4.06-05)` | letzten Snapshot aktualisieren | letzte Sequenz ist 6 |
| `TODO(4.06-06)` | Sequenzlücken erkennen | gesonderter Test in 4.07 |

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.06-sensor-pipeline/start -d build/day4-4.06-start
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.06-sensor-pipeline/solution -d build/day4-4.06-solution
./build/day4-4.06-solution/zephyr/zephyr.exe
```

Abnahme: `processed=4 drops=2`, letzte Sequenz 6 und `PIPELINE PASS`. Bearbeitungszeit:
etwa 45 Minuten.

