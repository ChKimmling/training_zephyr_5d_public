# 3.12 – Integrierter zbus Event Service

## Lernziel

Den vollständigen portablen Steuer- und Ereignispfad integrieren:

```text
Shell -> event_cmd_chan -> Stimulus Subscriber
      -> event_input_chan -> Async Listener -> Event Service
```

## Aufgabe

Vervollständige die TODOs in `start/` und nutze `solution/` als Referenz:

- Commands werden ausschließlich über `event_cmd_chan` gesendet.
- Der Subscriber verarbeitet Start/Stop/Reset und leitet Stimuli an den Adapter weiter.
- Input Events laufen über `event_input_chan`; fachliche Verarbeitung findet im Async Listener
  statt.
- GPIO- und Overlaydetails bleiben im Adapter. Shell-Handler enthalten kein GPIO-Wissen.
- Startzustand ist `stopped`; `event start` schaltet die Verarbeitung frei.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.12-event-service/start \
  -d build/day3-3.12
./build/day3-3.12/zephyr/zephyr.exe
```

Für die Referenz `start` durch `solution` und das Build-Verzeichnis durch
`build/day3-3.12-solution` ersetzen.

## Abnahme

```text
event reset
event status
event start
event input 1
event pulse
event burst 5 200
event status
event stats
event stop
event input 1
event stats
```

Shell-Kommandos melden die Publikation eines Commands; ihre Verarbeitung ist asynchron.
Prüfe Zähler daher nach Abschluss der Stimulusfolge erneut. Ein Input im gestoppten Zustand
wird sichtbar abgewiesen. Der Servicezustand ändert sich nicht durch Statusabfragen.

## Zählervertrag

- `cmd_published`: erfolgreich auf `event_cmd_chan` publizierte Commands.
- `cmd_received`: Commands, die der Stimulus Subscriber empfangen hat.
- `cmd_rejected`: ungültige, nicht zulässige oder im aktuellen Zustand abgewiesene Requests.
- `input_published`: erfolgreich publizierte Input Events.
- `input_received`: Input Events, die der Async Listener empfangen hat.
- `processed`: fachlich vollständig bearbeitete Input Events.
- `publish_failed`: fehlgeschlagene zbus-Publikationen.
- `last_error`: letzter relevante Fehlercode.

Ein logisches Input Event entspricht der aktiven Flanke; die inaktive Rückflanke beendet den
Stimulus, wird aber nicht als zweites Werkstückereignis gezählt. `event input 0` setzt den
Eingangspegel inaktiv und erzeugt daher kein Input Event.

Nach dem Stillstand aller asynchronen Verarbeitung gilt für einen fehlerfreien Ablauf:
`cmd_received <= cmd_published`, `input_received <= input_published` und
`processed <= input_received`. Abweichungen müssen durch Rejects oder Publish-Fehler erklärbar
sein. `event reset` setzt Zähler und `last_error` zurück, nicht den Start/Stop-Zustand oder
die Adapterkonfiguration.

## Erfolgskriterien

- Start und Solution bauen unabhängig mit `native_sim`.
- Shell, Command Subscriber und Input Listener bleiben klar getrennt.
- Shell-Aufrufe blockieren nicht auf Pulse oder Bursts.
- ungültige Parameter publizieren keinen Command.
- Servicefehler werden in Shell, Status oder Statistik nachvollziehbar.
- nach `event stop` werden neue Input-Anforderungen gemäß obiger Policy abgewiesen.
