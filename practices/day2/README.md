# Praktische Übungen – Tag 2

Die vier Übungen bauen auf den Kernel-Grundlagen des Tages auf. Jedes Projekt ist separat
baubar und enthält ein Teilnehmergerüst in `start/` mit kommentierten TODOs und
Funktionsgerüsten sowie eine Referenz in `solution/`.

| Agenda | Übung | Schwerpunkt |
|---|---|---|
| 2.06 | [Scheduling sichtbar machen](2.06-scheduling-exercise/README.md) | Prioritäten, Blockieren und freiwillige Abgabe |
| 2.09 | [Race Condition beheben](2.09-race-condition-exercise/README.md) | Mutex, Semaphore und kritische Abschnitte |
| 2.11 | [Producer–Consumer-Pipeline](2.11-producer-consumer-exercise/README.md) | Message Queue und Überlast |
| 2.13 | [Integrierte Mini-Anwendung](2.13-integrated-application/README.md) | Timer, Workqueue, Queue und Statistik |

## Voraussetzungen und Befehlskonventionen

Die Befehle sind aus dem Workspace-Root `training-zephyr-ws/` auszuführen. Aktiviere vorher
das Virtual Environment mit West. Ohne installiertes Zephyr SDK kannst du den Host-Compiler
verwenden:

```sh
export ZEPHYR_TOOLCHAIN_VARIANT=host
```

Jede README enthält vollständige Build- und Startbefehle für die jeweilige Übung. Die
Anwendungen verwenden `native_sim` und benötigen keine zusätzliche Hardware.
