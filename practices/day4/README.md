# Praktische Übungen – Tag 4

Tag 4 baut den **Portable Motion Monitor** schrittweise auf: Sensorzugriff, portable
Messprobe, Queue-Verarbeitung, Tests, Konnektivitätszustand und TCP-Statusausgabe.
Pflichtziel ist `native_sim`; Sensorwerte stammen aus dem BMI160-I²C-Emulator.
Alle Pflichtübungen funktionieren ohne Entwicklungsboard.

| Agenda | Übung | Form und Schwerpunkt |
|---|---|---|
| 4.02 / 4.03 | [Stationscheck und Sensor API](4.03-sensor-api-walkthrough/README.md) | Demo; Devicetree, Bus, Emulator und Fetch/Get |
| 4.04 | [Emulierten Sensor erfassen](4.04-sensor-read/README.md) | Start/Lösung; Sensorzugriff und Fehlerpfad |
| 4.05 | [Portable Probe-Struktur](4.05-motion-probe/README.md) | Start/Lösung; Datenvertrag, Qualität und Alter |
| 4.06 | [Sensor-Pipeline](4.06-sensor-pipeline/README.md) | Start/Lösung; Queue, Überlast und Snapshot |
| 4.07 | [Probe- und Queue-Tests](4.07-probe-tests/README.md) | Start/Lösung; Ztest und Twister |
| 4.09 | [Network Management](4.09-network-management-walkthrough/README.md) | Demo; L4-Events und Workqueue |
| 4.10 | [Konnektivitätsgesteuerte Anwendung](4.10-connectivity-state/README.md) | Start/Lösung; State und Offline-Erfassung |
| 4.11 | [Vorbereiteter TCP-Transport](4.11-prepared-tcp-transport/README.md) | Referenz; Socket-Vertrag und Cleanup |
| 4.12 | [Sensorstatus über TCP](4.12-tcp-status-integration/README.md) | Start/Lösung; Snapshot und Wiederverbindung |
| 4.13 | [Tagesabnahme](4.13-day-acceptance/README.md) | Referenz und Abnahmeskript; Diagnose und Übergabe |

## Voraussetzungen und Befehle

Die Befehle sind aus dem Workspace-Root `training-zephyr-ws/` auszuführen. Aktiviere das
Virtual Environment mit West. Ohne Zephyr SDK kann der Host-Compiler verwendet werden:

```sh
export ZEPHYR_TOOLCHAIN_VARIANT=host
```

Builds werden mit `-p always` und getrennten Build-Verzeichnissen durchgeführt.
Bei Bedarf kann bei manuellen Builds das Target `native_sim/native/64` verwendet werden;
das Abnahmeskript aus 4.13 verwendet `native_sim`.
Laufende Anwendungen kontrolliert mit Ctrl+C beenden. Ein beendetes `main()` bedeutet
nicht zwingend, dass auch der Simulatorprozess endet.

## Arbeitsweise und Abnahme

Beginne eine Programmierübung in `start/` und bearbeite ihre nummerierten
`TODO(4.xx-NN)`-Stellen. Vergleiche anschließend mit `solution/`.
Jeder Startstand ist unabhängig baubar; Dateien aus einer vorherigen Lösung müssen nicht
manuell kopiert werden. Einige Startstände zeigen nur einen kleinen Ablauf, während die
Referenz zusätzliche Prüfsequenzen enthält. Die READMEs unterscheiden diese ausdrücklich.

Demos und Referenzen besitzen keine künstlichen Start-/Lösungspaare. Der Stationscheck
4.02 nutzt das Projekt aus 4.03. 4.13 verwendet als Tagesreferenz den Stand aus 4.12
und ergänzt eine Abnahmekampagne.

Jedes Übungs-README beschreibt Lernziel, Aufgabe, Build und Start, Abnahmesequenz,
Erfolgskriterien sowie typische Fehler. Prüfe neben PASS-Marken auch Messwerte,
Zähler und Fehlerpfade. Zeitstempel und nebenläufige Logreihenfolgen sind keine starren
Sollwerte. Netzwerkdemos nutzen Loopback und benötigen kein TAP-Setup.

## Ergänzende Unterlagen

- [Sensor API](../../exkurs/sensor-api.md)
- [Network Management und Connection Manager](../../exkurs/network-manager.md)

`nrf52840dongle/nrf52840`, ESP32-C5-DevKitC-1 und BLE sind optionale Transferziele
und gehören nicht zur Pflichtabnahme.
