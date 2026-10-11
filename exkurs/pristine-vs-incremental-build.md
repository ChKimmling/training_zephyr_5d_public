# Pristine Build vs. Incremental Build

Zephyr legt beim Bauen in einem **Build-Verzeichnis** generierte Dateien und den
CMake-Konfigurationszustand ab. Dazu gehören unter anderem die ausgewählte Anwendung,
das Board, Kconfig-Werte, Toolchain-Informationen sowie generierte Devicetree-Dateien.
Ob ein Build inkrementell weiterlaufen kann oder frisch konfiguriert werden muss, hängt
davon ab, ob dieser gespeicherte Zustand noch zur gewünschten Konfiguration passt.

## Incremental Build

Ein inkrementeller Build verwendet das bestehende Build-Verzeichnis weiter und baut nur
neu, was sich seit dem letzten Build geändert hat. CMake und Ninja erkennen gewöhnliche
Änderungen an Quellcode, Kconfig und Devicetree-Dateien und aktualisieren die betroffenen
Schritte automatisch.

```sh
west build -b native_sim <app> -d build/my-app
```

Das ist der normale, schnelle Ablauf beim Entwickeln. Verwende für fortlaufende Builds
dieselbe Anwendung, dasselbe Board und dieselbe Toolchain.

## Pristine Build

Ein Pristine Build entfernt den zwischengespeicherten Build-Zustand und konfiguriert die
Anwendung von Grund auf neu. Das Build-Verzeichnis wird dabei geleert und neu erzeugt;
Quellcode und Dateien außerhalb dieses Build-Verzeichnisses werden nicht gelöscht.

```sh
west build -p always -b native_sim <app> -d build/my-app
```

`-p always` erzwingt bei jedem Aufruf einen Pristine Build.

## Wann ist ein Pristine Build zwingend?

- **Das Board bzw. das Build-Ziel wird gewechselt**, aber dasselbe Build-Verzeichnis soll
  weiterverwendet werden. Board, SoC und Architektur sind Teil der CMake-Konfiguration;
  dafür ist ein separates Build-Verzeichnis oder ein Pristine Build erforderlich.
- **Die Toolchain-Konfiguration wird gewechselt**, etwa `ZEPHYR_TOOLCHAIN_VARIANT` oder
  der verwendete Compiler. Der Build-Cache kann sonst weiterhin auf die alte Toolchain
  verweisen. Auch nach einem Toolchain-Wechsel ist ein frischer Build die sichere Wahl.
- **Zephyr-/Modulrevision oder grundlegende Build-Konfiguration wurde geändert** und der
  bestehende Build-Zustand ist damit nicht mehr kompatibel. Wenn CMake oder West explizit
  einen Pristine Build verlangt, muss der Build-Cache verworfen werden.
- **Der Build-Cache ist ungültig oder inkonsistent**, zum Beispiel nach einem
  fehlgeschlagenen oder abgebrochenen CMake-Konfigurationslauf, oder es erscheinen
  unerklärliche Fehler, die nach einer frischen Konfiguration verschwinden.

Für unterschiedliche Boards oder Varianten sind getrennte Build-Verzeichnisse oft besser
als wiederholtes Löschen:

```sh
west build -b native_sim <app> -d build/native-sim
west build -b <anderes-board> <app> -d build/anderes-board
```

**Meist nicht nötig:** Für normale Änderungen an C-Quellcode, Kconfig-Einstellungen oder
Devicetree-Overlays reicht ein inkrementeller Build. Auch beim Ändern von Overlay-Aliasen
oder Eigenschaften werden die generierten Devicetree-Dateien im normalen Build neu
erzeugt. Wenn VS Code anschließend alte IntelliSense-Ergebnisse zeigt, zuerst sicherstellen,
dass erfolgreich gebaut wurde und der Editor genau dieses Build-Verzeichnis verwendet;
ein Pristine Build ist dafür nicht automatisch erforderlich.
