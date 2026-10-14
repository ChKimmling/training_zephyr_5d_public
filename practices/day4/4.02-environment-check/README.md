# 4.02 – Arbeitsplatz-, Emulator- und Projektprüfung

## Lernziel und Rolle im Tagesprojekt

Der Stationscheck qualifiziert den verbindlichen `native_sim`-Pfad. Das vollständige
Referenzprojekt verwendet einen BMI160 am emulierten I²C-Bus und liest einen bekannten
Beschleunigungsvektor über die Zephyr Sensor API.

Für diesen Prüfpunkt gibt es bewusst kein künstliches `start`/`solution`-Paar.

## Build und Ausführung

Aus dem Workspace-Root:

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.02-environment-check/reference \
  -d build/day4-4.02
./build/day4-4.02/zephyr/zephyr.exe
```

Alternativ führt das Skript Build, Smoke-Test und zwei Artefaktprüfungen aus:

```sh
sh training-zephyr/practices/day4/4.02-environment-check/scripts/check.sh .
```

## Prüfpunkte

1. `motion_sensor: bmi160@68` in `build/day4-4.02/zephyr/zephyr.dts` finden.
2. `CONFIG_SENSOR=y`, `CONFIG_I2C=y` und `CONFIG_EMUL=y` in `.config` prüfen.
3. Die Zeile `DAY4_SMOKE PASS` beobachten.
4. Prüfen, dass X ungefähr 0, Y ungefähr 1000 und Z ungefähr 2000 mm/s² liefert.

## Abnahme

- frischer Build erfolgreich,
- Sensorgerät bereit,
- Emulatorwert über Sensor API gelesen,
- stabile PASS-Marke vorhanden,
- keine reale Hardware erforderlich.

Je nach Host kann `native_sim/native/64` anstelle von `native_sim` erforderlich sein.
