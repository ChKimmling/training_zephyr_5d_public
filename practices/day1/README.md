# Source-Übungen – Tag 1

Die verbleibenden Projekte behandeln die Agenda-Punkte 1.09 bis 1.13. Jedes Projekt enthält
ein eigenständiges `start/`-Projekt und wo passend, eine Referenzlösung.

| Agenda | Übung | Schwerpunkt |
|---|---|---|
| 1.09 | [Anatomie einer Anwendung](1.09-application-anatomy/README.md) | Header, Module und CMake |
| 1.10 | [Eigene minimale Anwendung](1.10-minimal-application/README.md) | Anwendung selbst erstellen |
| 1.11 | [Konfiguration mit Kconfig](1.11-kconfig/README.md) | Compile-Time-Konfiguration |
| 1.12 | [Devicetree in native_sim](1.12-devicetree/README.md) | Overlay und Devicetree-Makros |
| 1.13 | [Overlay und Diagnose](1.13-diagnostics/README.md) | Fehler in DTS, Kconfig, C und CMake |

## Voraussetzungen und Befehlskonventionen

Die Befehle sind aus dem Workspace-Root `training-zephyr-ws/` auszuführen. Die Python-
Umgebung mit West muss aktiviert sein. Falls kein Zephyr SDK installiert ist, verwenden die
Beispiele den Host-Compiler:

```sh
export ZEPHYR_TOOLCHAIN_VARIANT=host
```

Jede Übung baut nach `build/day1-<nummer>`. Ein Pristine-Build löscht ausschließlich das
jeweilige Build-Verzeichnis. Beispiel für 1.09:

```sh
west build -p always -b native_sim training-zephyr/practices/day1/1.09-application-anatomy/start \
  -d build/day1-1.09
./build/day1-1.09/zephyr/zephyr.exe
```

Für die übrigen Übungen wird in den jeweiligen Anleitungen nur der Anwendungs- und
Build-Verzeichnispfad angepasst.
