# 4.12 – Sensorstatus über TCP abfragen

## Lernziel

Den vorbereiteten TCP-Transport an genau einen konsistenten Anwendungs-Snapshot anbinden und nach einer Netzwerkunterbrechung einen aktuellen Status liefern.

## Aufgabe

Sensor-Pipeline und Connectivity-Modul sind vollständig. Ergänze die TODOs in
`src/main.c`, `src/status_provider.c` und `src/app_state.c`. Der Transport aus 4.11
bleibt hinter seiner öffentlichen Schnittstelle.

Der Startstand öffnet noch keinen Listener und enthält keinen Testclient.
Die Referenz zeigt einen internen Loopback-Client mit zwei Sitzungen auf Port 4243.
Nutze diesen Prüfablauf als Vorlage für die Prüfung des eigenen Stands. Der Client
sendet `GET\n` absichtlich in zwei Teilen. Damit wird sichtbar, dass TCP einen
Byte-Stream und keine Folge vollständiger Nachrichten überträgt.

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

## Prüfsequenz

1. Erste Probe erfassen und den Listener erst bei erreichtem L4-Zustand öffnen.
2. Einen Snapshot lesen und in stabiler Feldreihenfolge formatieren.
3. Erste `GET\n`-Anfrage bearbeiten: Antwort mit Sequenz 1 erwarten.
4. Disconnect auslösen, weiter erfassen und nach Reconnect dieselbe Listener-Instanz
   für eine zweite Sitzung verwenden: Antwort mit Sequenz 2 erwarten.
5. Antworten auf Sequenz, drei Achsen, Qualität, Alter, Drops, Sensorfehler und
   Connectivity prüfen. Zeit- und Quantisierungswerte nicht starr vergleichen.
6. Zusätzlich einen zu kleinen Formatpuffer prüfen: `-ENOSPC` erwarten.

Die Referenz meldet `TCP_STATUS PASS`, wenn beide Antworten die erwartete Sequenz,
`quality=OK` und `connected=1` enthalten, zwei Anfragen gezählt wurden und der letzte
Snapshot Sequenz 2 besitzt. Lese-, Schreib- und Semaphore-Wartezeiten sind begrenzt. Ein
Peer-Close-Fehlerfall bleibt eine mögliche Vertiefung.

## Erfolgskriterien

- Alle Antwortfelder stammen aus einer einzigen Snapshot-Kopie.
- Formatierung meldet fehlende Probe bzw. Pufferüberlauf als Fehler.
- Erfolgreiche Anfragen und Fehler werden getrennt unter dem State-Lock gezählt.
- Sensorzustand schreitet während Disconnect fort.
- Zwei Sitzungen sind ohne Prozessneustart möglich; Sitzung und Listener werden geschlossen.

## Hinweise und typische Fehler

- 4.12 ergänzt den Transport aus 4.11 um zeilenorientiertes Lesen des TCP-Bytestroms.
- Ein Client mit irgendeiner empfangenen Nutzlast beweist noch keinen vollständigen Status.
- Netzwerkevents vor der abhängigen Transportaktion zuverlässig verarbeiten lassen.
- `recv()` kann auch bei kurzen Anfragen und Antworten nur einen Teil der Daten liefern.
- Der kleine Antwortpuffer wird vor dem Netzwerkablauf geprüft und muss `-ENOSPC` liefern.

## Optional: nRF7002 DK

Mit „nrf72 DevKit“ ist hier das in Zephyr 4.4.2 vorhandene **nRF7002 DK** gemeint.
Das Target für den nRF5340-Anwendungskern lautet `nrf7002dk/nrf5340/cpuapp`.

Das Board besitzt in der verwendeten Zephyr-Boarddefinition keinen Bewegungssensor.
Der optionale Transfer ersetzt deshalb nur die Sensorquelle durch kontrollierte
synthetische Werte. TCP bleibt lokaler IPv4-Loopback auf dem Mikrocontroller. Dadurch
werden Portabilität, Scheduler, Speicher und der Zephyr-Netzwerkstack auf echter
Hardware geprüft, aber weder ein physischer Sensor noch WLAN-Kommunikation.

```sh
unset ZEPHYR_TOOLCHAIN_VARIANT
west build -p always -b nrf7002dk/nrf5340/cpuapp \
  training-zephyr/practices/day4/4.12-tcp-status-integration/solution \
  -d build/day4-4.12-nrf7002dk
west flash -d build/day4-4.12-nrf7002dk
```

Der Startstand kann mit demselben Target und einem eigenen Buildverzeichnis gebaut
werden; im Quellpfad wird dazu `solution` durch `start` ersetzt.

Die UART-Ausgabe kann beispielsweise mit 115200 Baud über `/dev/ttyACM0` gelesen
werden. Erwartet werden zwei Statusantworten und `TCP_STATUS PASS`. Der
boardspezifische Test-Zufallszahlengenerator ist ausschließlich für TCP-Sequenznummern
im lokalen Trainings-Loopback aktiviert und nicht für kryptografische oder produktive
Anwendungen geeignet.

Ein echter WLAN-Transfer ist eine eigenständige Erweiterung: Er benötigt den
nRF7002-WLAN-Treiber, sichere Bereitstellung der Zugangsdaten, DHCP beziehungsweise
eine statische Adresse, Bindung an die WLAN-Schnittstelle und einen externen Client.
