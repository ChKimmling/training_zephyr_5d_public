# 4.03 – Sensor API, Bus und Emulator

## Lernziel

Erklären, wie Devicetree, Bus, BMI160-Treiber, Emulator und Sensor API zusammenarbeiten. Einen bekannten Beschleunigungsvektor über Fetch/Get lesen und in Anwendungseinheiten einordnen.

## Aufgabe

`demo/` ist ein vollständiger Walkthrough ohne Teilnehmer-TODOs. Es dient zugleich als
Stationscheck für Agendapunkt 4.02. Ein eigenes Referenzprojekt für 4.02 entfällt.

1. In `boards/native_sim.overlay` Alias, Busadresse und Sensorknoten lokalisieren.
2. In `src/main.c` Gerätebereitschaft, Messbereich und Emulatorstimulus nachvollziehen.
3. Fetch, XYZ-Channel und Konvertierung voneinander unterscheiden.
4. Erklären, welche Aufgaben der Emulator und welche der Anwendungscode übernehmen.

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.03-sensor-api-walkthrough/demo -d build/day4-4.03
./build/day4-4.03/zephyr/zephyr.exe
```

## Abnahmesequenz

1. Demo frisch bauen und starten.
2. `motion_sensor: bmi160@68` in `build/day4-4.03/zephyr/zephyr.dts` suchen.
3. `CONFIG_SENSOR=y`, `CONFIG_I2C=y` und `CONFIG_EMUL=y` in `.config` prüfen.
4. Ausgabe mit X ungefähr 0, Y ungefähr 1000 und Z ungefähr 2000 mm/s² vergleichen.
5. Die Messwertausgabe und vorangehende Diagnosemeldungen gemeinsam bewerten.

Der Stationscheck für 4.02 wird anhand dieser Schritte manuell durchgeführt.

## Erfolgskriterien

- Sensorgerät ist bereit und der bekannte Stimulus wird über die Sensor API gelesen.
- Kleine Abweichungen durch Sensorquantisierung und Ganzzahlkonvertierung sind erklärbar.
- Fetch aktualisiert den Treiberzwischenspeicher; Get liest einen Channel daraus.
- Der Stationscheck benötigt keine reale Hardware.

## Hinweise und typische Fehler

- Fehlender Alias oder deaktivierter Knoten: Overlay und erzeugtes Devicetree prüfen.
- Gerät nicht bereit: Treiberkonfiguration und Initialisierung untersuchen.
- Falsche Einheit: Beschleunigung aus der API kommt in m/s²; die Ausgabe verwendet mm/s².

Vertiefung: [Sensor API](../../../exkurs/sensor-api.md).
