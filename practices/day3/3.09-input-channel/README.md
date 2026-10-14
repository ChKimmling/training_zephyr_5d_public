# 3.09 – Commands in Input Events übersetzen

## Lernziel

Die Bedien-Commands von den portierbaren Eingabeereignissen trennen. Ein Message Subscriber
empfängt `event_command` und publiziert das daraus erzeugte `event_input` auf
`event_input_chan`.

## Aufgabe

Arbeite in `start/` die TODOs in `src/event_subscriber.c` ab:

1. Warte auf Commands über `event_cmd_chan`.
2. Erzeuge aus einem akzeptierten `EVENT_CMD_INPUT` ein Input Event mit Quelle und Sequenz.
3. Prüfe den Rückgabewert von `zbus_chan_pub`.
4. Zähle ein Input Event nur nach erfolgreicher Publikation als `input_published`.
5. Halte den Message Subscriber unabhängig von GPIO und Targetdetails.

Die kleine Anwendung publiziert beim Start einen Test-Command. Die Referenz gibt die
ankommende Nachricht aus; ein GPIO-Adapter ist nicht erforderlich.

## Build

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.09-input-channel/start \
  -d build/day3-3.09
./build/day3-3.09/zephyr/zephyr.exe
```

## Abnahme

Ein gültiger Test-Command muss genau ein `event_input` erzeugen. Quelle und Sequenznummer
müssen in der Diagnose wiedererkennbar sein. Ein fehlgeschlagener Publish darf nicht als
erfolgreiches Input Event zählen.
