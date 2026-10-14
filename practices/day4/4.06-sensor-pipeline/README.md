# 4.06 – Sensor-Pipeline mit Message Queue

## Lernziel

Vollständige Proben über eine Message Queue weitergeben, Überlast kontrolliert behandeln und einen geschützten Anwendungs-Snapshot mit nachvollziehbaren Zählern pflegen.

## Aufgabe

Die Sensorquelle und Probe-Erzeugung sind vollständig. Ergänze `src/probe_queue.c`
und `src/app_state.c`. Die Queue besitzt vier Plätze; „neuester Wert gewinnt“ bedeutet:
Bei voller Queue den ältesten Eintrag entfernen und die neue Probe einstellen.
Der Rückgabewert 1 meldet einen Drop an den Aufrufer, der den Zähler erhöht.

Der Startstand erfasst eine Probe. Die Referenz erzeugt sechs Proben und leert danach
die Queue. Beide Schritte laufen hier nacheinander in `main()`; separate Producer- und
Consumer-Threads sind eine mögliche Erweiterung.

| TODO | Auftrag | Lösungsnachweis |
|---|---|---|
| `TODO(4.06-01)` | nicht blockierend einstellen | `k_msgq_put(..., K_NO_WAIT)` |
| `TODO(4.06-02)` | Überlastpolicy umsetzen | sechs Eingaben ergeben zwei Drops |
| `TODO(4.06-03)` | Fehler- und Dropzähler schützen | Snapshot enthält stabile Zähler |
| `TODO(4.06-04)` | Consumer entnimmt vollständige Probe | vier Proben verarbeitet |
| `TODO(4.06-05)` | letzten Snapshot aktualisieren | letzte Sequenz ist 6 |
| `TODO(4.06-06)` | Sequenzlücken erkennen | gesonderter Test in 4.07 |

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.06-sensor-pipeline/start -d build/day4-4.06-start
./build/day4-4.06-start/zephyr/zephyr.exe

west build -p always -b native_sim \
  training-zephyr/practices/day4/4.06-sensor-pipeline/solution -d build/day4-4.06-solution
./build/day4-4.06-solution/zephyr/zephyr.exe
```

## Abnahmesequenz

1. Zunächst eine Probe einstellen und entnehmen: `processed=1`, keine Drops.
2. Den Ablauf mit sechs Eingaben aus der Referenz auf den eigenen Stand übertragen.
3. Erst nach allen Eingaben entnehmen: Verarbeitet werden Sequenzen 3, 4, 5 und 6.
4. Erwartete Bilanz: `processed=4`, `drops=2`, letzte Sequenz 6.
5. Die Referenz erwartet außerdem `gaps=0` und meldet `PIPELINE PASS`.
6. Separat Sequenzen 1 und 4 akzeptieren: zwei fehlende Sequenzen nachweisen.

Die zwei vor der ersten Verarbeitung verworfenen Proben zählen als Drops. Der
Sequenzlückenzähler beginnt erst mit einer bereits bekannten vorherigen Probe.

## Erfolgskriterien

- Publikation wartet nicht auf freie Queueplätze.
- Überlast entfernt genau einen ältesten Eintrag pro erfolgreichem Ersatz.
- Zählerupdates und Snapshot-Kopien verwenden den State-Lock.
- Consumer übernimmt vollständige Proben.
- Queue-Drops und Sequenzlücken bleiben getrennte, erklärbare Größen.

## Hinweise und typische Fehler

- Rückgabewert 1 ist eine erfolgreiche Publikation mit Drop, kein Sensorfehler.
- Drop nicht zugleich im Queue-Modul und im Aufrufer zählen.
- Ein einzelner normaler Durchlauf prüft noch keine Überlast.
- Der sequenzielle Referenzablauf beweist keine korrekte Nebenläufigkeit; dafür wären
  zusätzliche Producer-/Consumer-Szenarien erforderlich.
