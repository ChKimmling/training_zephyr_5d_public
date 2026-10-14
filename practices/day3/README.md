# Praktische Übungen – Tag 3

Die Übungen bauen eine interaktive Event-Anwendung schrittweise auf. Jedes Projekt ist
unabhängig baubar und enthält ein Teilnehmergerüst in `start/` sowie eine Referenz in
`solution/`.

| Agenda | Übung | Schwerpunkt |
|---|---|---|
| 3.02 | [Logging aktivieren und gezielt filtern](3.02-logging-basics/README.md) | Logging-Modul und Log-Level |
| 3.05 | [Zephyr Shell aktivieren und erkunden](3.05-shell-setup/README.md) | Shell-Konfiguration und Standardkommandos |
| 3.07 | [zbus-Steuerkanal](3.07-event-shell/README.md) | Commands über `event_cmd_chan` |
| 3.08 | [zbus-Vertrag](3.08-event-flow/README.md) | Nachrichtentypen und Channel-Vertrag |
| 3.09 | [Input-Channel](3.09-input-channel/README.md) | Commands in portable Input Events übersetzen |
| 3.10 | [Stimuli per Shell](3.10-shell-stimulus/README.md) | Nicht blockierende Einzel- und Sequenzstimuli |
| 3.11 | [Asynchrone Verarbeitung](3.11-async-processing/README.md) | Async Listener und Verarbeitungskontexte |
| 3.12 | [Integrierter Event Service](3.12-event-service/README.md) | Vollständiger zbus-Ereignispfad |
| 3.13 | [Portabilität](3.13-portability/README.md) | Gleicher Vertrag in zwei Targetvarianten |

## Voraussetzungen und Befehle

Die Befehle sind aus dem Workspace-Root `training-zephyr-ws/` auszuführen. Aktiviere vorher
das Virtual Environment mit West. Ohne Zephyr SDK kann für `native_sim` der Host-Compiler
verwendet werden:

```sh
export ZEPHYR_TOOLCHAIN_VARIANT=host
```

Build- und Startbefehle sowie Shell-Abnahmesequenzen stehen jeweils im README. Die
zbus-Übungen halten Nachrichtentypen und Channel-Namen ab 3.08 stabil. Alle Pflichtübungen
verwenden `native_sim`; für die Hauptübungen ist keine Hardware erforderlich. Beende laufende
Anwendungen mit Ctrl+C.
