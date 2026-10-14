# 4.12 – Sensorstatus über TCP abfragen

Der vorbereitete TCP-Transport aus 4.11 wird an den konsistenten `app_state`-Snapshot
angebunden. Ein interner Loopback-Client führt zwei getrennte Sitzungen aus.

| TODO | Auftrag | Nachweis |
|---|---|---|
| `TODO(4.12-01)` | Listener an L4-Zustand koppeln | kein Start ohne Connectivity |
| `TODO(4.12-02)` | einen Snapshot lesen | keine gemischten Feldstände |
| `TODO(4.12-03)` | stabiles Statusformat | Sequenz, Achsen, Qualität, Alter, Zähler |
| `TODO(4.12-04)` | Requests und Fehler zählen | getrennte Zähler |
| `TODO(4.12-05)` | zweite Sitzung nach Reconnect | zwei erfolgreiche Clients |

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.12-tcp-status-integration/solution \
  -d build/day4-4.12-solution
./build/day4-4.12-solution/zephyr/zephyr.exe
```

Abnahme: beide Antworten enthalten einen vollständigen Status, die zweite Antwort besitzt
Sequenz 2 und `TCP_STATUS PASS` erscheint. Bearbeitungszeit: etwa 30 Minuten.

