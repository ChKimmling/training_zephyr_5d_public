# 4.13 – Integrierte Tests, Diagnose und Tagesabnahme

## Lernziel

Den Tagesstand anhand von Build, Laufzeitverhalten, Modultests und einer nachvollziehbaren Diagnosekette bewerten und für Tag 5 sichern.

## Aufgabe

`reference/` entspricht vollständig der Lösung aus 4.12. Der neue Schwerpunkt ist die
Abnahmekampagne; es gibt kein weiteres Start-/Lösungspaar.
`scripts/check.sh` baut ausschließlich diese Referenz und startet die Lösungstests aus 4.07.
Ein bestandener Skriptlauf nimmt deshalb nicht automatisch den eigenen Teilnehmerstand ab.
Die erwarteten Invarianten stehen in `fixtures/expected.txt`.

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.13-day-acceptance/reference -d build/day4-4.13
./build/day4-4.13/zephyr/zephyr.exe
```

Referenzkampagne einschließlich Twister:

```sh
sh training-zephyr/practices/day4/4.13-day-acceptance/scripts/check.sh .
```

## Abnahmesequenz

1. Referenz mit dem Skript frisch bauen und prüfen: `TCP_STATUS PASS` und anschließend
   `Tag 4 acceptance: PASS` erwarten; Twister-Ergebnis einschließlich ausgeführter Fälle lesen.
2. Den eigenen vollständigen Stand separat frisch bauen und dieselbe TCP-Sequenz durchführen.
3. Die selbst implementierten Tests aus 4.07 ausführen, nicht nur deren Referenzlösung.
4. Bereichsgrenzen, Sensorfehler, Queue-Überlast und Sequenzlücken gezielt nachweisen.
5. Offline-Erfassung, Reconnect, Antwortinhalt und Deskriptor-Cleanup untersuchen.
6. Kommandos, Ergebnisse und offene Abweichungen in einer Übergabenotiz festhalten.

Das Skript prüft die PASS-Marke der TCP-Anwendung und die Modultests. Es vergleicht
`fixtures/expected.txt` nicht automatisch; weitere Diagnose- und Fehlerfälle sind manuell
bzw. mit ergänzten Tests nachzuweisen. Eine automatische `STALE`-Bewertung ist im
aktuellen Probe-Modul nicht implementiert.

## Erfolgskriterien

- Eigener Stand und Referenz sind mit getrennten Build-Verzeichnissen geprüft.
- Queue-Policy und Zählerbilanz sind durch die passenden Modultests nachgewiesen.
- Sensorfortschritt und Statusabruf nach Disconnect sind nachvollziehbar.
- Offene Grenzen bei Events, Framing und Timeout sind dokumentiert.
- Übergabe an Tag 5 enthält Target, Build-/Testkommandos und den tatsächlich geprüften Stand.

## Hinweise und typische Fehler

- Diagnosefolge: Build/Kconfig → Devicetree → Emulator → Sensor API → Probe → Queue
  → `app_state` → Netzwerkevents → TCP.
- PASS-Marke nicht mit vollständiger Fehlerfallabdeckung gleichsetzen.
- Ein äußeres Prozesszeitlimit ersetzt keinen Socket-Timeout.
- Devkit- und BLE-Transfers gehören nicht zur Pflichtabnahme.
