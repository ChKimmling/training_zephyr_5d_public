# 1.13 – Praxis 4: Overlay und Diagnose

## Ziel

Buildfehler systematisch einer Schicht zuordnen, Ursache finden, minimal korrigieren und
erneut bauen.

## Ausgangssituation

`cases/` enthält sechs voneinander getrennte Projekte mit je genau einem gezielt eingebauten
Fehler. Zu jedem Fall gibt es eine funktionierende Korrektur unter `solutions/`.

## Aufgabe

Bearbeite alle sechs Fälle. Für jeden Fall:

1. Baue das Projekt und finde die erste relevante Fehlermeldung.
2. Ordne sie DTS, Devicetree-Generierung, C, Kconfig oder CMake/Linker zu.
3. Dokumentiere Fehlermeldung und Ursache.
4. Nimm die kleinstmögliche Korrektur vor.
5. Baue erneut und starte die Anwendung.

| Fall | Fehlerklasse | Hinweis |
|---|---|---|
| `01-invalid-dts-syntax` | DTS-Syntax | Fehlendes Satzzeichen |
| `02-missing-alias` | Devicetree-Referenz | C verweist auf den falschen Alias |
| `03-disabled-node` | Knotenstatus | Der benötigte Knoten ist deaktiviert |
| `04-wrong-property-type` | Eigenschaftstyp | Eine Zahl wird als Text angegeben |
| `05-missing-config` | Kconfig | Erforderliches Symbol fehlt in `prj.conf` |
| `06-source-not-in-cmake` | CMake/Linker | Implementierung ist nicht im Build |

## Vorgehen

Beispiel für Fall 1, aus dem Workspace-Root:

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day1/1.13-diagnostics/cases/01-invalid-dts-syntax \
  -d build/day1-1.13-01
```

Nutze für jeden Fall ein eigenes Build-Verzeichnis. Die Befehle liefern für die absichtlich
fehlerhaften Startfälle einen Fehler; baue die zugehörige Lösung zum Gegencheck:

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day1/1.13-diagnostics/solutions/01-invalid-dts-syntax \
  -d build/day1-1.13-solution-01
./build/day1-1.13-solution-01/zephyr/zephyr.exe
```

Nach der Beobachtung jede gestartete `native_sim`-Anwendung mit Ctrl+C beenden.

## Erwartetes Ergebnis

Die fehlerhaften Projekte scheitern jeweils in der erwarteten Schicht. Die sechs
Referenzlösungen bauen und geben `diagnostic case recovered` aus.
