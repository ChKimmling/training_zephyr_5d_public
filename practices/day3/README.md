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

## Gemeinsame UART für Shell und Tracing

Das optionale Overlay `serial-tracing.overlay` legt für `native_sim` Konsole, serielle Shell
und UART-Tracing auf `uart0`. Hänge es beim Build einer Tag-3-Übung mit
`-- -DEXTRA_DTC_OVERLAY_FILE=training-zephyr/practices/day3/serial-tracing.overlay` an. Das
Overlay wählt nur das Gerät aus; UART-Tracing muss zusätzlich über Kconfig aktiviert sein.

Bei `native_sim` ist `uart0` ein PTY-UART. Die Meldung mit `/dev/pts/<N>` ist daher
erwartet: Das Overlay wählt die Zephyr-UART aus, ändert aber nicht, wie native_sim sie an den
Host bindet. Für die interaktive Shell ist dieses PTY der vorgesehene Anschluss. Die Kconfig-Option
`CONFIG_UART_NATIVE_PTY_0_ON_STDINOUT` leitet stattdessen auf stdin/stdout um; das ist laut
Zephyr-Dokumentation für interaktive Shells nicht empfohlen. In Übung 3.07 ist UART-Tracing
außerdem nicht per Kconfig aktiviert; die Devicetree-Auswahl `zephyr,tracing-uart` allein startet
kein Tracing.

Shell-Text und UART-Tracing teilen sich damit denselben Datenkanal. Bei gleichzeitigem
interaktivem Shellbetrieb und UART-Trace können Text und Trace-Daten ineinanderlaufen; für
eine zuverlässige Trace-Aufzeichnung sollten Shell und Tracing getrennte Schnittstellen
verwenden.
