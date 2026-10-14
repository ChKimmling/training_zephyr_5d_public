# 3.10 – Stimuli per Shell

## Lernziel

Portierbare Input Events über Shell-Commands anfordern und Pulse/Bursts asynchron ausführen,
während die Shell bedienbar bleibt.

## Aufgabe

`start/` enthält dieselben öffentlichen Nachrichten und Shell-Kommandos wie `solution/`.
Implementiere die markierten TODOs so, dass die Shell `event_command` auf `event_cmd_chan`
publiziert. Ein Subscriber übersetzt Input, Pulse und Burst in logische Nachrichten auf
`event_input_chan`. Nutze Delayable Work oder einen vergleichbaren Zustandsautomaten; der
Shell-Handler darf nicht schlafen.

Die Pflichtlösung erzeugt Input Events deterministisch und ohne reale Hardware. Der optionale
GPIO-Adapter bleibt hinter derselben portablen Schnittstelle. Ungültige Eingaben dürfen keinen
Command publizieren. Während einer Sequenz werden manuelle Pegeländerungen und neue Sequenzen
mit `-EBUSY` abgewiesen; Status und Statistik bleiben abrufbar.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.10-shell-stimulus/start \
  -d build/day3-3.10
./build/day3-3.10/zephyr/zephyr.exe
```

Die Referenzlösung lässt sich mit demselben Befehl durch Ersetzen von `start` durch `solution`
in einem eigenen Build-Verzeichnis bauen:

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.10-shell-stimulus/solution \
  -d build/day3-3.10-solution
./build/day3-3.10-solution/zephyr/zephyr.exe
```

`start/` baut und startet bereits, aber noch nicht implementierte Stimulus-Kommandos melden
deutlich, dass die TODOs in `stimulus.c` fehlen.

## Abnahmesequenz

```text
event status
event input 1
event input 0
event input 2
event input 10
event pulse 0
event pulse 19
event pulse 20
event pulse 2001
event burst 0 100
event burst 21 100
event pulse
event status
event burst 3 200
event burst 2 100
event stats
event burst 20 2000
event pulse
event input 1
event status
event cancel
event status
event stats
```

Referenzgrenzen: Pulsdauer 20–2000 ms (Standard 100 ms), Burst 1–20 logische Events,
Abstand 20–2000 ms. `event input` akzeptiert ausschließlich 0 oder 1; nur ein Übergang auf
den aktiven Pegel erzeugt ein Input Event, der Rückweg auf `0` beendet den Pegel ohne ein
zweites fachliches Ereignis. Während
`event burst 3 200` muss die Shell auf `event status` und `event stats` reagieren. Die Folge
`event burst 20 2000`, danach `event pulse` und `event input 1`, prüft Busy-Verhalten;
`event cancel` beendet den Ablauf.

Jeder akzeptierte `event pulse` erzeugt ein logisches Input Event; `event burst N` erzeugt
genau N logische Events. Nach Ende der asynchronen Verarbeitung gilt für erfolgreich
publizierte Events `input_published = input_received = processed`. Physische GPIO-Flanken
sind Adapterdetails und keine fachlichen Produktzähler.

## Erfolgskriterien

- Emulierter Eingang ist über die Shell setzbar.
- Pulse und Bursts sind nicht blockierend; Status bleibt währenddessen abrufbar.
- Falsche Syntax und Wertebereiche werden abgewiesen, ohne einen Ablauf zu starten.
- Callback hält die Arbeit kurz und gibt keine Shell-Ausgabe aus.
- Eingangsereignisse und Verarbeitung sind getrennt zählbar.
- Start- und Lösungsprojekt verwenden dieselbe Shell-Oberfläche, Overlay-Definition und API.
- Aktive Sequenzen enden durch Abschluss oder Abbruch mit inaktivem Eingangspegel.
