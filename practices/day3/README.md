# Praktische Übungen – Tag 3

Die Übungen bauen eine interaktive Event-Anwendung schrittweise auf. Jedes Projekt ist
unabhängig baubar und enthält ein Teilnehmergerüst in `start/` sowie eine Referenz in
`solution/`.

| Agenda | Übung | Schwerpunkt |
|---|---|---|
| 3.02 | [Logging aktivieren und gezielt filtern](3.02-logging-basics/README.md) | Logging-Modul und Log-Level |
| 3.05 | [Zephyr Shell aktivieren und erkunden](3.05-shell-setup/README.md) | Shell-Konfiguration und Standardkommandos |
| 3.07 | [Status- und Bedienkommandos](3.07-event-shell/README.md) | Eigene Shell-Kommandos und Service-API |
| 3.08 | [Ereignispfad für den Use-Case planen](3.08-event-flow/README.md) | Kontrollpfad, Ereignispfad und Zählersemantik |
| 3.10 | [GPIO-Emulation per Shell triggern](3.10-shell-stimulus/README.md) | Reproduzierbare GPIO-Events ohne Hardware |
| 3.12 | [Interaktiver Event Service](3.12-event-service/README.md) | Shell, Stimulus, Callback und asynchrone Verarbeitung |

## Voraussetzungen und Befehle

Die Befehle sind aus dem Workspace-Root `training-zephyr-ws/` auszuführen. Aktiviere vorher
das Virtual Environment mit West. Ohne Zephyr SDK kann für `native_sim` der Host-Compiler
verwendet werden:

```sh
export ZEPHYR_TOOLCHAIN_VARIANT=host
```

Build- und Startbefehle sowie Shell-Abnahmesequenzen stehen jeweils im README. Übung 3.08 ist
eine Planungsübung ohne eigenes Build. Die übrigen Anwendungen verwenden `native_sim`; für die
Hauptübungen ist keine Hardware erforderlich. Beende sie mit Ctrl+C.
