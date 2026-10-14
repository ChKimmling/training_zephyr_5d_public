# 4.07 – Ztest, Twister und Fehler-Injektion

## Lernziel

Probe- und Queue-Verhalten mit unabhängigen Ztest-Fällen prüfen und mit Twister reproduzierbar ausführen.

## Aufgabe

Probe, Queue und `app_state` sind vollständig aus 4.06 übernommen. Ergänze ausschließlich
die Testaufgaben in `src/main.c`; `testcase.yaml` beschreibt die Suite für Twister.
Die unveränderten Starttests werden mit `ztest_test_skip()` übersprungen. Ein Lauf mit
übersprungenen Pflichtfällen gilt nicht als bearbeitete Übung.

Die Suite prüft Fachlogik und Module mit direkten Eingabedaten. Sie baut keinen
Sensortreiber auf und ist kein Emulatorintegrationstest.

| TODO | Testziel | Nachweis in der Lösung |
|---|---|---|
| `TODO(4.07-01)` | Fixture zurücksetzen | Before-Hook leert Queue und Zustand |
| `TODO(4.07-02)` | gültig und Bereichsgrenze | fachliche Assertions |
| `TODO(4.07-03)` | Sensorfehler | Qualität und genullte Achsen |
| `TODO(4.07-04)` | Queue-Überlast | zwei Drops, neueste Sequenz 6 |
| `TODO(4.07-05)` | Sequenzlücke | Sequenzen 1 und 4 ergeben zwei Lücken |

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west twister -T training-zephyr/practices/day4/4.07-probe-tests/start \
  -p native_sim --inline-logs --outdir build/day4-4.07-twister-start
west twister -T training-zephyr/practices/day4/4.07-probe-tests/solution \
  -p native_sim --inline-logs --outdir build/day4-4.07-twister-solution
```

## Abnahmesequenz

1. Before-Hook implementieren und jeden Test mit leerer Queue und zurückgesetztem State starten.
2. Fachliche Assertions ergänzen; die Skip-Aufrufe in den Pflichtfällen entfernen.
3. Suite für den eigenen Startstand und anschließend für die Referenz ausführen.
4. Sicherstellen, dass alle vorgesehenen Fälle ausgeführt wurden und keine Assertion fehlschlägt.
5. Vorübergehend einen fachlichen Defekt im eigenen Stand einbauen, etwa eine falsche
   Bereichsgrenze; der passende Test muss fehlschlagen. Defekt anschließend zurücknehmen.

Die Referenz enthält zusätzlich einen Alterstest. Ihr Bereichstest prüft aktuell eine
gültige Probe und +20001; ergänze für den vollständigen Grenznachweis genau ±20000
und −20001. Beim Sensorfehler alle drei Achsen prüfen, nicht nur X.

## Erfolgskriterien

- Pflichtfälle werden ausgeführt und besitzen fachlich aussagekräftige Assertions.
- Tests laufen unabhängig von Reihenfolge und vorherigen Queue-Inhalten.
- Überlast ergibt zwei Drops, vier verarbeitete Proben und letzte Sequenz 6.
- Sequenzen 1 und 4 ergeben zwei Lücken.
- Fehler- und Alterssemantik stimmen mit dem Probe-Vertrag überein.

## Hinweise und typische Fehler

- Eine grüne Suite mit übersprungenen Tests ist kein Nachweis der Implementierung.
- Qualität allein genügt nicht: relevante Nutzdaten und Zähler ebenfalls prüfen.
- Eigene Twister-Ausgabeverzeichnisse für Start und Referenz verwenden.
- Zeitstempel und zufällige Logreihenfolgen nicht als starre Sollwerte verwenden.
