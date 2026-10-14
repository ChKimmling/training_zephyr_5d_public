# 3.08 – zbus-Vertrag festlegen

## Lernziel

Portierbare Command- und Input-Nachrichten sowie die beiden Channel-Schnittstellen
implementieren. Diese Namen und Payloads bleiben in den folgenden Übungen unverändert.

## Aufgabe

Ergänze in `start/` die markierten TODOs und gleiche sie mit `solution/` ab:

1. Definiere `event_command` mit Command-Typ, Sequenznummer und Parametern.
2. Definiere `event_input` mit Quelle, Sequenznummer, logischem Zustand und Zeitstempel.
3. Deklariere `event_cmd_chan` und `event_input_chan` in einem öffentlichen Header.
4. Definiere beide Channels mit passenden Initialwerten.
5. Halte GPIO- und Boardtypen aus den Nachrichtentypen heraus.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.08-event-flow/start \
  -d build/day3-3.08
./build/day3-3.08/zephyr/zephyr.exe
```

Für die Referenz `start` durch `solution` ersetzen und ein eigenes Build-Verzeichnis
verwenden.

## Verbindlicher Vertrag

- `event_cmd_chan`: Shell-Producer, Stimulus-Subscriber-Consumer.
- `event_input_chan`: Stimulus-Producer, Async-Listener-Consumer.
- `event reset` setzt vereinbarte Statistikfelder und `last_error` zurück, ändert aber keine
  Adapter- oder Boardkonfiguration.
- Status und Statistik verwenden ab Übung 3.07 dieselben benannten Felder.
- Ungültige Shell-Eingaben publizieren keinen Command.

## Erfolgskriterien

- Start und Solution bauen unabhängig für `native_sim`.
- Beide Channels und Payloads sind in öffentlichen Headern deklariert.
- Payloads enthalten keine GPIO-Spezifikation und sind targetunabhängig.
- Channel-Namen und Nachrichtentypen werden in späteren Übungen nicht mehr geändert.
