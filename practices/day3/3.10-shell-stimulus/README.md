# 3.10 – GPIO-Emulation per Shell triggern

## Lernziel

GPIO-Events ohne reale Hardware auslösen und Pulse/Bursts asynchron erzeugen, während die
Shell weiter bedienbar bleibt.

## Aufgabe

`start/` enthält dieselben Dateien und öffentlichen APIs wie `solution/`. Die Event-Shell,
GPIO-Eingabe und Statistikabfrage sind vorbereitet. Implementiere die kommentierten TODOs in
`start/src/stimulus.c`: Emulator-Eingabe, Status-Snapshot und nicht blockierende Puls- und
Burst-Sequenzen. Verwende Delayable Work oder einen vergleichbaren Zustandsautomaten; der
Shell-Handler darf nicht schlafen. Der Starter-Stub hält den Pegel zunächst inaktiv und
meldet für noch nicht implementierte Stimulusbefehle `-ENOSYS`.

Das Overlay stellt über den Alias `event-input` einen Eingang am nativen GPIO-Emulator bereit.
Validiere Eingaben vollständig, bevor du den Pegel änderst oder Arbeit einplanst. Während eine
Sequenz läuft, werden manuelle Pegeländerungen und neue Sequenzen mit `-EBUSY` abgewiesen;
Status und Statistik bleiben abrufbar.

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

Referenzgrenzen: Pulsdauer 20–2000 ms (Standard 100 ms), Burst 1–20 Pulse,
Abstand 20–2000 ms **zwischen dem Ende eines Pulses und dem nächsten Puls**. Im Burst ist
jeder Puls 100 ms aktiv. Die erste positive Flanke erfolgt beim Annehmen der Sequenz; der
Aufruf `event pulse 20` erzeugt daher genau zwei rohe GPIO-Flanken. `event input` ist nur im
Zustand `idle` erlaubt. Während `event burst 3 200` sollte die Shell auf `event status` und
`event stats` reagieren. Die Folge `event burst 20 2000`, anschließend `event pulse` und
`event input 1`
prüft Busy-Verhalten; `event cancel` beendet den Burst und stellt den inaktiven Pegel wieder
her.

Nach einer vollständig abgearbeiteten Pulssequenz gilt `processed = raw`. Weil beide GPIO-
Flanken gezählt werden, erzeugt ein Puls zwei Zähleinträge und ein Fünfer-Burst zehn. Während
einer Sequenz darf `processed` asynchron hinter `raw` liegen; in `start/` und `solution/`
verarbeitet ein Worker-Thread diese Flanken.

## Erfolgskriterien

- Emulierter Eingang ist über die Shell setzbar.
- Pulse und Bursts sind nicht blockierend; Status bleibt währenddessen abrufbar.
- Falsche Syntax und Wertebereiche werden abgewiesen, ohne einen Ablauf zu starten.
- Callback hält die Arbeit kurz und gibt keine Shell-Ausgabe aus.
- Eingangsereignisse und Verarbeitung sind getrennt zählbar.
- Start- und Lösungsprojekt verwenden dieselbe Shell-Oberfläche, Overlay-Definition und API.
- Aktive Sequenzen enden durch Abschluss oder Abbruch mit inaktivem Eingangspegel.
