# Praktische Übungen – Tag 4

Tag 4 baut den **Portable Motion Monitor** auf. Pflichtziel ist `native_sim`; alle Übungen
funktionieren ohne Entwicklungsboard. Sensorwerte stammen aus dem BMI160-I²C-Emulator.

| Agenda | Form | Schwerpunkt |
|---|---|---|
| 4.02 | Referenzcheck | Workspace, Devicetree, Emulator und Sensor API |
| 4.03 | Demo | Fetch–Get–Convert–Validate |
| 4.04 | Start/Lösung | Sensorzugriff und Fehlerpfad |
| 4.05 | Start/Lösung | portable Probe und Qualitätsmodell |
| 4.06 | Start/Lösung | Message Queue, Überlast und `app_state` |
| 4.07 | Start/Lösung | Ztest und Twister |
| 4.09 | Demo | `net_mgmt` und Connection Manager |
| 4.10 | Start/Lösung | konnektivitätsgesteuerte Anwendung |
| 4.11 | Referenz | vorbereiteter TCP-Transport |
| 4.12 | Start/Lösung | TCP-Statusintegration |
| 4.13 | Abnahme | vollständiger Tagesstand |

Die Befehle werden aus `training-zephyr-ws/` ausgeführt. Bei Verwendung des Host-Compilers:

```sh
export ZEPHYR_TOOLCHAIN_VARIANT=host
```

Startstände enthalten nummerierte Pflichtaufgaben im Format `TODO(4.xx-NN)`. Die jeweilige
Lösung enthält keine Pflicht-TODOs und implementiert alle in der README aufgeführten Punkte.
Für 4.02, 4.03, 4.09, 4.11 und 4.13 werden bewusst keine künstlichen Start-/Lösungspaare
angelegt.

`nrf52840dongle/nrf52840`, ESP32-C5-DevKitC-1 und BLE sind ausschließlich optionale
Transferziele. Sie gehören nicht zur Pflichtabnahme.

