# Praktische Übungen – Tag 5

Die Übungen verfolgen den technischen Weg vom Zephyr-Start über prüfbare
Firmware-Images bis zu Update, C++ und Power Management. Pflichtziel ist
`native_sim`; alle Projekte funktionieren ohne Entwicklungsboard.

| Agenda | Übung | Schwerpunkt |
|---|---|---|
| 5.04 | [Bootpfad eines Zephyr-Images](5.04-boot-path/README.md) | Referenz; ELF, Map-Datei, Symbole und Initialisierungslevel |
| 5.06 | [Zephyr-Image signieren und prüfen](5.06-sign-image/README.md) | Referenz; `imgtool`, `west sign`, Header, TLVs und Manipulationsnachweis |
| 5.10 | [Update, Bestätigung und Revert](5.10-update-state-analysis/README.md) | Referenz; Slotabbilder, Trailerzustände und Rollback-Regel |
| 5.12 | [C++-Wrapper für eine C-Schnittstelle](5.12-cpp-wrapper/README.md) | Start/Lösung und C-Referenz; Besitz, Fehlerstatus, Symbole und Größe |
| 5.13 | [Device Runtime Power Management](5.13-power-management/README.md) | Start/Lösung; Gerätezustände, Referenzzählung und Busy-Markierung |

## Voraussetzungen und Befehle

Die Befehle sind aus dem Workspace-Root `training-zephyr-ws/` auszuführen. Aktiviere
vorher das Virtual Environment mit West. Ohne installiertes Zephyr SDK kann für
`native_sim` der Host-Compiler verwendet werden:

```sh
source .venv/bin/activate
export ZEPHYR_TOOLCHAIN_VARIANT=host
```

Jede README enthält die vollständigen Build-, Start- und Analysebefehle für die
jeweilige Übung. Für die Artefaktanalyse werden außerdem die Hostwerkzeuge `readelf`,
`nm`, `objdump` und `grep` verwendet.

Übung 5.06 verwendet ausschließlich einen lokal erzeugten Trainingsschlüssel. Private
Schlüssel und erzeugte Images bleiben unter `build/` und werden nicht eingecheckt.

## Arbeitsweise

Startstände enthalten nummerierte Aufgaben im Format `TODO(5.xx-NN)`. Die jeweilige
Lösung enthält keine Aufgaben-TODOs und implementiert alle in der README beschriebenen
Punkte. Analyseübungen verwenden stattdessen ein vollständiges Projekt unter
`reference/`; dafür werden bewusst keine künstlichen Start-/Lösungspaare angelegt.

Reale Boot-ROM-Eigenschaften, Hardwarelatenzen und Energieverbrauch gehören nicht zum
Nachweis mit `native_sim`. Entwicklungsboards sind für die Übungen nicht erforderlich.

