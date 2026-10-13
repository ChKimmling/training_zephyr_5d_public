# 3.05 – Zephyr Shell aktivieren und erkunden

## Lernziel

Die Shell für `native_sim` konfigurieren, die effektive Build-Konfiguration prüfen und
Standardkommandos zur Laufzeitdiagnose verwenden.

## Ausgangslage und Aufgabe

`start/` ist eine ausführbare Anwendung ohne Shell. Das Logging aus Übung 3.02 bleibt
aktiviert. Ergänze die in `start/prj.conf` kommentierten Shell-Optionen, baue und starte
die Anwendung erneut. Aktiviere die Shell, das serielle Backend sowie die Device-, Kernel-
und GPIO-Shell-Module. `solution/` enthält dieselbe Anwendung mit genau diesen Optionen
aktiviert.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.05-shell-setup/start \
  -d build/day3-3.05
./build/day3-3.05/zephyr/zephyr.exe
```

Baue die Referenzlösung separat:

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.05-shell-setup/solution \
  -d build/day3-3.05-solution
./build/day3-3.05-solution/zephyr/zephyr.exe
```

Ohne Shell sind in `start/` nur die Startmeldung und keine Eingabeaufforderung zu erwarten.
Nach Aktivierung der Optionen und in `solution/` erscheint eine Shell-Eingabeaufforderung.
Beende die laufende Anwendung mit Ctrl+C.

## Shell-Abnahmesequenz

```text
help
device list
kernel uptime
kernel thread list
gpio devices
```

Probiere in der Shell zusätzlich Tab-Vervollständigung und `history` aus. Prüfe in
`build/day3-3.05/zephyr/.config` (beziehungsweise `build/day3-3.05-solution/zephyr/.config`
für die Referenz), welche Optionen tatsächlich aktiv sind. Ein aktiviertes GPIO-Shell-Modul
garantiert nicht, dass das Devicetree einen GPIO-Controller bereitstellt; `gpio devices` kann
daher eine leere Liste anzeigen. Geräte- und Threadlisten sowie Zeitwerte hängen von Target
und Konfiguration ab.

## Erwartete Beobachtungen

- `help` listet die registrierten Befehle; `device`, `kernel` und `gpio` sind verfügbar.
- `device list` zeigt die registrierten Geräte des Targets.
- `kernel uptime` und `kernel thread list` zeigen Laufzeit- beziehungsweise Threadzustand.
- `gpio devices` zeigt nur dann Controller, wenn das Target passende GPIO-Geräte bereitstellt.
- Tab-Vervollständigung und `history` werden von der Shell bereitgestellt.
- In `.config` sind `CONFIG_SHELL`, `CONFIG_SHELL_BACKEND_SERIAL`,
  `CONFIG_DEVICE_SHELL`, `CONFIG_KERNEL_SHELL`, `CONFIG_GPIO` und `CONFIG_GPIO_SHELL`
  wirksam.

## Erfolgskriterien

- Das Teilnehmerprojekt startet vor und nach der Shell-Aktivierung.
- Die Shell ist über die native_sim-Konsole bedienbar und `help` funktioniert.
- Mindestens ein Geräte- und ein Kernelzustand lassen sich untersuchen.
- Targetabhängige Ausgaben werden nicht mit einer festen Referenzausgabe verwechselt.
- Logging bleibt wie in Übung 3.02 aktiviert und gibt die Startmeldung aus.
