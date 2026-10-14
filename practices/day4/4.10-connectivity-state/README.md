# 4.10 – Konnektivitätsgesteuerte Anwendung

Die Sensor-Pipeline ist vollständig. Implementiert wird die Übergabe von L4-Events aus dem
Callback in den Workqueue-Kontext. Deterministische Lehrstimuli verwenden denselben
`net_mgmt`-Eventpfad wie reale Connection-Manager-Ereignisse.

| TODO | Auftrag | Nachweis |
|---|---|---|
| `TODO(4.10-01)` | initialen Zustand setzen und Status anfordern | definierter Startzustand |
| `TODO(4.10-02)` | L4-Events filtern | Connect/Disconnect unterscheidbar |
| `TODO(4.10-03)` | Zustand per Work Item übergeben | Callback bleibt kurz |
| `TODO(4.10-04)` | Zustand idempotent setzen | doppelte Events ohne Transition |
| `TODO(4.10-05)` | Transitionen zählen | 2 Connects, 1 Disconnect |
| `TODO(4.10-06)` | während Disconnect erfassen | Sequenz erreicht 3 |

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.10-connectivity-state/solution \
  -d build/day4-4.10-solution
./build/day4-4.10-solution/zephyr/zephyr.exe
```

Das Projekt verwendet den internen Loopback-Treiber und benötigt kein TAP-Setup.

