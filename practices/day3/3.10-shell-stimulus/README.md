# 3.10 – GPIO-Emulation per Shell triggern

## Lernziel

GPIO-Events ohne reale Hardware auslösen und Pulse/Bursts asynchron erzeugen, während die
Shell weiter bedienbar bleibt.

## Aufgabe

Erweitere die vorhandene Event-Shell mit `status`, `stats` und `reset`. Das Overlay stellt
über den Alias `event-input` einen Eingang am nativen GPIO-Emulator bereit. Implementiere
`event input <0|1>`, `event pulse [dauer_ms]` und `event burst <anzahl> <abstand_ms>`.
Eingaben müssen vollständig geparst und
begrenzt sein. Ein weiterer Stimulus während einer Sequenz wird mit `-EBUSY` abgewiesen.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.10-shell-stimulus/start \
  -d build/day3-3.10
./build/day3-3.10/zephyr/zephyr.exe
```

Für die Referenz baue `solution/` nach `build/day3-3.10-solution`.

## Abnahmesequenz

```text
event status
event input 1
event input 0
event input 2
event pulse 0
event pulse 2001
event burst 0 100
event pulse
event status
event burst 20 2000
event burst 2 100
event stats
event cancel
```

Referenzgrenzen: Pulsdauer 20–2000 ms (Standard 100 ms), Burst 1–20 Pulse,
Abstand 20–2000 ms **zwischen dem Ende eines Pulses und dem nächsten Puls**. Jeder Puls ist
100 ms aktiv. Der lange Burst hält die Sequenz für den Busy-Test aktiv; ein zweiter Start wird
mit `-EBUSY` abgewiesen. `event cancel` beendet sie kontrolliert und stellt den Ruhezustand
wieder her.

## Erfolgskriterien

- Emulierter Eingang ist über die Shell setzbar.
- Pulse und Bursts sind nicht blockierend; Status bleibt währenddessen abrufbar.
- Falsche Syntax und Wertebereiche werden abgewiesen, ohne einen Ablauf zu starten.
- Callback hält die Arbeit kurz und gibt keine Shell-Ausgabe aus.
- Eingangsereignisse und Verarbeitung sind getrennt zählbar.
