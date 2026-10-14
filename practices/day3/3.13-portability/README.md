# 3.13 – Portabilität nachweisen

## Lernziel

Nachweisen, dass Nachrichten, Channels und Fachverarbeitung unabhängig von der
Devicetree-Variante bleiben. Nur Overlay und Stimulusadapter dürfen targetabhängig sein.

## Aufgabe

Baue beide Varianten pristine und führe dieselbe Shell-Sequenz auf beiden aus:

```text
event reset
event status
event start
event input 1
event pulse
event burst 5
event status
event stats
event stop
```

Vergleiche die Zähler und benenne die geänderten Adapter-/Boarddateien. Die öffentliche
Definition von `event_command`, `event_input`, `event_cmd_chan` und `event_input_chan` muss
unverändert bleiben.

## Build

Die Projektdateien verwenden die integrierte Anwendung aus 3.12; die beiden Overlays in
dieser Übung variieren nur die GPIO-Belegung und Polarität.

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.13-portability/start \
  -d build/day3-3.13-a
./build/day3-3.13-a/zephyr/zephyr.exe

west build -p always -b native_sim training-zephyr/practices/day3/3.13-portability/solution \
  -d build/day3-3.13-b
./build/day3-3.13-b/zephyr/zephyr.exe
```

## Vergleich auf dem nRF52840 Dongle

Auch für das Hardwareziel gibt es je ein Overlay. `start/` verwendet GPIO0.13 als
aktiv-high-Eingang mit internem Pull-down. Dafür einen externen Taster zwischen P0.13 und
VDD anschließen. `solution/` verwendet den vorhandenen Taster SW1 an P1.6, aktiv-low mit
Pull-up. Die logische Event-Schnittstelle bleibt in beiden Varianten unverändert. In der
Dongle-Referenzlösung werden `event input`, `event pulse` und `event burst` vom
Stimulusadapter explizit mit `-ENOTSUP` beantwortet: GPIO-Eingänge lassen sich nicht wie
die `native_sim`-Emulation per Software auslösen. Für den Hardwarevergleich den Taster
betätigen und `event stats` verwenden; die Starteradapter-TODOs bleiben wie vorgesehen
unimplementiert.

```sh
west build -p always -b nrf52840dongle/nrf52840 \
  training-zephyr/practices/day3/3.13-portability/start -d build/day3-3.13-dongle-start
west build -p always -b nrf52840dongle/nrf52840 \
  training-zephyr/practices/day3/3.13-portability/solution -d build/day3-3.13-dongle-solution
```

Die beiden Builds dienen dem Vergleich; zum Flashen des Dongles sind ein unterstützter
Programmierweg und die passende Board-Bootloader-Konfiguration erforderlich.

## Erfolgskriterien

- beide Varianten bauen für `native_sim`.
- Shell-Oberfläche und zbus-Vertrag sind identisch.
- dieselbe logische Eingabe erzeugt dieselbe fachliche Zählerbilanz.
- Unterschiede bleiben auf Overlay, Konsole und Adapter begrenzt.
- nicht unterstützte Adapterfunktionen melden explizit `-ENOTSUP`.
