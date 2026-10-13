# 3.02 – Logging aktivieren und gezielt filtern

## Lernziel

Das Zephyr-Logging aktivieren, ein Anwendungsmodul registrieren und die Wirkung eines
modulspezifischen Info- beziehungsweise Debug-Levels vergleichen.

## Ausgangslage und Aufgabe

`start/` ist eine lauffähige Anwendung, die vier Meldungen mit `printk()` ausgibt. Die
Kommentare in `src/main.c` ordnen jeder Meldung einen passenden Log-Level zu. Überführe die
Ausgaben in der Referenzlösung in das Logging-Subsystem und ergänze die Konfiguration.

1. Aktiviere Logging. `native_sim` stellt dafür automatisch das native POSIX-Backend bereit.
2. Registriere das Modul `event_app` und ersetze `printk()` durch `LOG_INF`, `LOG_DBG`,
   `LOG_WRN` und `LOG_ERR`.
3. Baue und starte die Info- und Debug-Variante.
4. Prüfe den wirksamen Modul-Level in `zephyr/.config` und ordne die Ausgabe den Meldungen zu.

## Build und Start

Die Befehle sind vom Workspace-Root `training-zephyr-ws/` auszuführen.

Startversion ohne Logging:

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.02-logging-basics/start \
  -d build/day3-3.02-start
./build/day3-3.02-start/zephyr/zephyr.exe
```

Referenz, Info-Level:

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.02-logging-basics/solution \
  -d build/day3-3.02-info
./build/day3-3.02-info/zephyr/zephyr.exe
```

Referenz, Debug-Level:

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.02-logging-basics/solution \
  -d build/day3-3.02-debug -- -DEXTRA_CONF_FILE=logging-debug.conf
./build/day3-3.02-debug/zephyr/zephyr.exe
```

Die Anwendungen bleiben nach der Ausgabe aktiv. Beende sie mit Ctrl+C.

## Erwartete Meldungen

| Meldung | Level | Info-Build | Debug-Build |
|---|---|---|---|
| Event Service startet | Info | sichtbar | sichtbar |
| Eingangsstatus beim Start | Debug | nicht sichtbar | sichtbar |
| Optionale Statusausgabe fehlt; Fortsetzung möglich | Warnung | sichtbar | sichtbar |
| Beispiel-Fehlerpfad: ungültige Event-Anforderung abgewiesen | Fehler | sichtbar | sichtbar |

Die letzte Meldung ist ein deterministisches Beispiel für einen Fehlerpfad, kein Fehler beim
Start der Anwendung.

## Konfiguration prüfen

Vergleiche die erzeugten Konfigurationen:

```sh
grep -E 'CONFIG_(LOG=|LOG_BACKEND_NATIVE_POSIX=|LOG_DEFAULT_LEVEL=|LOG_MAX_LEVEL=|EVENT_APP_LOG_LEVEL=)' \
  build/day3-3.02-info/zephyr/.config \
  build/day3-3.02-debug/zephyr/.config
```

`CONFIG_LOG_BACKEND_NATIVE_POSIX=y` bestätigt das für `native_sim` verwendete Backend.
`CONFIG_LOG_DEFAULT_LEVEL` ist der globale Standard für Module ohne eigene Vorgabe.
`CONFIG_EVENT_APP_LOG_LEVEL` setzt den Level des registrierten `event_app`-Moduls; der
Debug-Build ändert nur diesen Modul-Level. `CONFIG_LOG_MAX_LEVEL` begrenzt, welche Levels
überhaupt einkompiliert werden.

## Erfolgskriterien

- `start/` baut und zeigt die vier unstrukturierten `printk()`-Meldungen.
- Die Lösung registriert `event_app` und verwendet für alle Meldungen passende Log-Makros.
- Im Info-Build ist Debug ausgefiltert; im Debug-Build wird die Debug-Meldung sichtbar.
- Die `.config`-Dateien belegen den Unterschied zwischen den beiden Builds.
- Die Ausgabe enthält Modulname, Level und Meldung; die Zeitinformation kann je nach
  Zephyr-Formatter variieren.

Beim Übergang zu 3.05 bleibt Logging für interne Diagnose aktiv. Die Shell ergänzt eine
Bedienoberfläche, ersetzt aber keine Logmeldung.
