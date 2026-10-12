# 3.05 – Zephyr Shell aktivieren und erkunden

## Lernziel

Die Shell für `native_sim` konfigurieren, die effektive Build-Konfiguration prüfen und
Standardkommandos zur Laufzeitdiagnose verwenden.

## Ausgangslage und Aufgabe

`start/` ist eine ausführbare Anwendung ohne Shell. Ergänze die Konfiguration in `prj.conf`,
baue und starte sie. Aktiviere schrittweise die Shell, das serielle Backend sowie die
Device-, Kernel- und GPIO-Shell-Module. Vergleiche mit `solution/`.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.05-shell-setup/start \
  -d build/day3-3.05
./build/day3-3.05/zephyr/zephyr.exe
```

Nutze für die Referenz `solution` statt `start` und ein eigenes Build-Verzeichnis, z. B.
`build/day3-3.05-solution`.

## Shell-Abnahmesequenz

```text
help
device list
kernel uptime
kernel thread list
gpio
```

Prüfe im Build-Verzeichnis `build/day3-3.05/zephyr/.config`, welche Optionen tatsächlich
aktiv sind. Ein aktiviertes Modul garantiert nicht, dass das Devicetree passende Hardware
bereitstellt. Welche Geräte und Unterkommandos sichtbar sind, hängt von Target und
Konfiguration ab.

## Erfolgskriterien

- Das Teilnehmerprojekt startet vor und nach der Shell-Aktivierung.
- Die Shell ist über die native_sim-Konsole bedienbar und `help` funktioniert.
- Mindestens ein Geräte- und ein Kernelzustand lassen sich untersuchen.
- Targetabhängige Ausgaben werden nicht mit einer festen Referenzausgabe verwechselt.
