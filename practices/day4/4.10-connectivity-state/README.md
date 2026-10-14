# 4.10 – Konnektivitätsgesteuerte Anwendung

## Lernziel

L4-Konnektivität in einen geschützten Anwendungszustand integrieren, echte Transitionen zählen und Sensorerfassung während einer Unterbrechung fortsetzen.

## Aufgabe

Die Sensor-Pipeline ist vollständig. Bearbeite `src/connectivity.c`,
`src/app_state.c` und den markierten Offline-Erfassungspfad in `src/main.c`.
Callback-Arbeit bleibt kurz; die State-Aktualisierung erfolgt über ein Work Item.
Der Startstand besitzt noch keine fertige Zustandsübernahme und erfasst offline keine Proben.
Loopback ersetzt das externe Netzwerk; TAP und reale Hardware sind nicht erforderlich.

| TODO | Auftrag | Nachweis |
|---|---|---|
| `TODO(4.10-01)` | initialen Zustand setzen und Status anfordern | definierter Startzustand |
| `TODO(4.10-02)` | L4-Events filtern | Connect/Disconnect unterscheidbar |
| `TODO(4.10-03)` | Zustand per Work Item übergeben | Callback bleibt kurz |
| `TODO(4.10-04)` | Zustand idempotent setzen | doppelte Events ohne Transition |
| `TODO(4.10-05)` | Transitionen zählen | 2 Connects, 1 Disconnect |
| `TODO(4.10-06)` | während Disconnect erfassen | Sequenz erreicht 3 |

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.10-connectivity-state/start -d build/day4-4.10-start
./build/day4-4.10-start/zephyr/zephyr.exe

west build -p always -b native_sim \
  training-zephyr/practices/day4/4.10-connectivity-state/solution -d build/day4-4.10-solution
./build/day4-4.10-solution/zephyr/zephyr.exe
```

## Abnahmesequenz

1. Initialen Zustand definieren, Callback registrieren und Monitorstatus anfordern.
2. Connect, Disconnect und Reconnect durch die vorbereiteten Lehrstimuli auslösen.
3. Während Disconnect drei Proben erfassen und über die Queue verarbeiten.
4. Für die kontrollierte Folge `connected=1`, `connect=2`, `disconnect=1`, `seq=3`
   und drei verarbeitete Proben erwarten. Die Referenz prüft die ersten vier Werte
   und meldet `CONNECTIVITY PASS`.
5. Zusätzlich denselben Zustand zweimal setzen: Der Transitionszähler darf beim
   zweiten Aufruf nicht steigen. Dies ist kein eigener Fall im Referenzablauf.

Bei abweichenden Zählern zuerst die Eventverarbeitung untersuchen:
`connectivity_wait_for_work()` bestätigt nur das Work Item. Noch ausstehende Netzwerkevents
und der neu angeforderte Monitorstatus können den synthetischen Ablauf beeinflussen.

## Erfolgskriterien

- Andere Events werden ignoriert; Connect und Disconnect sind unterscheidbar.
- State und Transitionszähler werden unter dem Lock aktualisiert.
- Wiederholter gleicher Zustand erhöht keinen Transitionszähler.
- Sensorsequenz schreitet während Disconnect fort.
- Fachlogik enthält keine technologieabhängigen Netzwerkdetails.

## Hinweise und typische Fehler

- Atomarer Zwischenzustand und ein Work Item können schnelle Änderungen zusammenfassen;
  sie speichern keine vollständige Ereignishistorie.
- Bei Bedarf jede Transition über eine Queue sichern und Überlauf definieren.
- Die Sequenznummer darf nicht erst beim Reconnect weiterlaufen.

Vertiefung: [Network Management und Connection Manager](../../../exkurs/network-manager.md).
