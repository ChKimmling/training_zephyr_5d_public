# 4.12 – Sensorstatus über TCP abfragen

## Lernziel

Den vorbereiteten TCP-Transport an genau einen konsistenten Anwendungs-Snapshot anbinden und nach einer Netzwerkunterbrechung einen aktuellen Status liefern.

## Aufgabe

Sensor-Pipeline und Connectivity-Modul sind vollständig. Ergänze die TODOs in
`src/main.c`, `src/status_provider.c` und `src/app_state.c`. Der Transport aus 4.11
bleibt hinter seiner öffentlichen Schnittstelle.

Der Startstand öffnet noch keinen Listener und enthält keinen Testclient.
Die Referenz zeigt einen internen Loopback-Client mit zwei Sitzungen auf Port 4243.
Nutze diesen Prüfablauf als Vorlage für die Abnahme des eigenen Stands.

| TODO | Auftrag | Nachweis |
|---|---|---|
| `TODO(4.12-01)` | Listener an L4-Zustand koppeln | kein Start ohne Connectivity |
| `TODO(4.12-02)` | einen Snapshot lesen | keine gemischten Feldstände |
| `TODO(4.12-03)` | stabiles Statusformat | Sequenz, Achsen, Qualität, Alter, Zähler |
| `TODO(4.12-04)` | Requests und Fehler zählen | getrennte Zähler |
| `TODO(4.12-05)` | zweite Sitzung nach Reconnect | zwei erfolgreiche Clients |

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.12-tcp-status-integration/start -d build/day4-4.12-start
./build/day4-4.12-start/zephyr/zephyr.exe

west build -p always -b native_sim \
  training-zephyr/practices/day4/4.12-tcp-status-integration/solution -d build/day4-4.12-solution
./build/day4-4.12-solution/zephyr/zephyr.exe
```

## Abnahmesequenz

1. Erste Probe erfassen und den Listener erst bei erreichtem L4-Zustand öffnen.
2. Einen Snapshot lesen und in stabiler Feldreihenfolge formatieren.
3. Erste `GET\n`-Anfrage bearbeiten: Antwort mit Sequenz 1 erwarten.
4. Disconnect auslösen, weiter erfassen und nach Reconnect dieselbe Listener-Instanz
   für eine zweite Sitzung verwenden: Antwort mit Sequenz 2 erwarten.
5. Antworten auf Sequenz, drei Achsen, Qualität, Alter, Drops, Sensorfehler und
   Connectivity prüfen. Zeit- und Quantisierungswerte nicht starr vergleichen.
6. Zusätzlich einen zu kleinen Formatpuffer prüfen: `-ENOSPC` erwarten.

Die Referenz meldet `TCP_STATUS PASS`, wenn zwei Clients Daten empfangen haben,
zwei Anfragen gezählt wurden und der letzte Snapshot Sequenz 2 besitzt.
Sie prüft den Antwortinhalt nicht vollständig und führt keinen Peer-Close-Fehlerfall aus.

## Erfolgskriterien

- Alle Antwortfelder stammen aus einer einzigen Snapshot-Kopie.
- Formatierung meldet fehlende Probe bzw. Pufferüberlauf als Fehler.
- Erfolgreiche Anfragen und Fehler werden getrennt unter dem State-Lock gezählt.
- Sensorzustand schreitet während Disconnect fort.
- Zwei Sitzungen sind ohne Prozessneustart möglich; Sitzung und Listener werden geschlossen.

## Hinweise und typische Fehler

- Grenzen des Transportmoduls aus 4.11 gelten auch hier.
- Ein Client mit irgendeiner empfangenen Nutzlast beweist noch keinen vollständigen Status.
- Netzwerkevents vor der abhängigen Transportaktion zuverlässig verarbeiten lassen.
- Fehler können die Referenz beim Warten auf `client_done` blockieren; bei ausbleibender
  Ausgabe kontrolliert beenden und die vorhergehenden Fehlerpfade untersuchen.
