# 5.04 – Bootpfad eines Zephyr-Images analysieren

## Lernziel

Den Startpfad einer Zephyr-Anwendung anhand von ELF-Header, Sections, Symboltabelle,
Map-Datei und der von Zephyr erzeugten Initialisierungsreihenfolge nachvollziehen.
Dabei die Host-Startumgebung von `native_sim` klar von einem Reset auf einem
Mikrocontroller unterscheiden.

## Einordnung

`reference/` ist ein vollständiges, unverändertes Analyseobjekt. Das Projekt registriert
je eine benannte Initialisierungsfunktion in den Leveln `EARLY`, `PRE_KERNEL_1`,
`PRE_KERNEL_2`, `POST_KERNEL` und `APPLICATION`. Die Funktionen schreiben ihre
Levelkennung ohne Konsolenzugriff in einen Ereignispuffer. `main()` gibt anschließend
Bitmaske und beobachtete Laufzeitreihenfolge aus.

Die Übung besitzt bewusst kein `start`-/`solution`-Paar: Es wird kein Quellcode ergänzt.
Untersucht werden der vollständige Referenzquelltext und die daraus erzeugten Artefakte.

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day5/5.04-boot-path/reference \
  -d build/day5-5.04
./build/day5-5.04/zephyr/zephyr.exe
```

Falls die verwendete Zephyr-Version einen qualifizierten Board-Namen verlangt, kann
`native_sim/native/64` verwendet werden.

Die Anwendung bestätigt mit der Bitmaske `0x1f`, dass alle fünf registrierten
Initialisierungsfunktionen vor `main()` ausgeführt wurden. Die zusätzlichen
`position`-Zeilen belegen ihre konkrete Laufzeitreihenfolge:

```text
BOOT_TRACE position=0 level=EARLY
BOOT_TRACE position=1 level=PRE_KERNEL_1
BOOT_TRACE position=2 level=PRE_KERNEL_2
BOOT_TRACE position=3 level=POST_KERNEL
BOOT_TRACE position=4 level=APPLICATION
BOOT_TRACE order=PASS
```

## Aufgabe

### 1. ELF-Header einordnen

```sh
readelf -h build/day5-5.04/zephyr/zephyr.exe
```

Bestimme ELF-Klasse, Zielarchitektur und Entry Point. Ordne den Entry Point anschließend
korrekt ein: `zephyr.exe` ist der fertige `native_sim`-Hostprozess. Sein Entry Point
beschreibt deshalb den Start des Hostprogramms und nicht den Resetvektor eines
Mikrocontrollers.

Vergleiche damit das Zephyr-Zwischenartefakt:

```sh
readelf -h build/day5-5.04/zephyr/zephyr.elf
```

Erkläre anhand des ELF-Typs, warum dessen Sections und Symbole noch keine endgültigen
Laufzeitadressen besitzen.

### 2. Speicherbereiche untersuchen

```sh
readelf -S -W build/day5-5.04/zephyr/zephyr.exe
```

Suche mindestens `.text`, `.rodata`, `.data` und `.bss`. Erkläre für jede Section:

- welche Art von Inhalt sie trägt,
- ob der Inhalt im ELF-Image gespeichert ist,
- ob zur Laufzeit Speicher dafür benötigt wird.

Vergleiche die Beobachtung mit den Größenangaben:

```sh
west build -d build/day5-5.04 -t footprint
```

### 3. Symbole im Startpfad lokalisieren

```sh
nm -n build/day5-5.04/zephyr/zephyr.exe | \
  grep -E 'boot_trace_|z_cstart| main$'
```

Ordne die Symbole aus `reference/src/main.c` den Adressen zu. Prüfe, welche weiteren
Startsymbole im ELF sichtbar sind. `native_sim` enthält sowohl den Einstieg des
Hostprogramms als auch Zephyrs `main()`. Eine Symboladresse beschreibt die Lage im
gebauten Programm, aber noch nicht automatisch die zeitliche Aufrufreihenfolge.

### 4. Map-Datei mit der Symboltabelle vergleichen

```sh
grep -n -E 'boot_trace_|z_cstart| main$' \
  build/day5-5.04/zephyr/zephyr.map
```

Die Map-Datei beschreibt bei `native_sim` das relocatable Zephyr-Zwischenartefakt.
Verwende sie, um Output-Section und beitragende Objektdatei zuzuordnen. Die dortigen
Zwischenadressen dürfen nicht direkt mit den endgültigen Adressen aus `zephyr.exe`
gleichgesetzt werden. Unterscheide dabei:

- Symboladresse,
- zugeordnete Output-Section,
- Objektdatei, aus der das Symbol stammt.

Untersuche danach die speziellen Initialisierungssections:

```sh
grep -n -E '\.z_init_(EARLY|PRE_KERNEL_1|PRE_KERNEL_2|POST_KERNEL|APPLICATION)' \
  build/day5-5.04/zephyr/zephyr.map
```

Ein Name wie `.z_init_POST_KERNEL_P_50_SUB_0_` codiert Level, Priorität und eine vom
Buildsystem verwendete Unterordnung. Diese Sections werden vom Linker in die
Initialisierungstabelle einsortiert. Die Reihenfolge der gewöhnlichen Funktionsadressen
in `.text` ist davon unabhängig.

### 5. Initialisierungsreihenfolge ausgeben

```sh
west build -d build/day5-5.04 -t initlevels
```

Suche die fünf `boot_trace_*`-Funktionen. Vergleiche ihre Reihenfolge mit den
`SYS_INIT()`-Deklarationen und der Laufzeitausgabe. Das Target wertet die
Initialisierungseinträge des fertigen ELF aus und zeigt zusätzlich die Einträge aus
`DEVICE_DEFINE()` und verwandten Makros. Es ist deshalb aussagekräftiger als eine reine
Quelltextsuche nach `SYS_INIT()`.

Für die Sortierung gelten diese Regeln:

1. Level werden als `EARLY`, `PRE_KERNEL_1`, `PRE_KERNEL_2`, `POST_KERNEL`,
   `APPLICATION` und bei aktiviertem SMP zuletzt `SMP` durchlaufen.
2. Innerhalb eines Levels wird eine kleinere Prioritätszahl früher ausgeführt.
3. Prioritäten liegen zwischen 0 und 999 und müssen als dezimales Literal oder einzelnes
   Makro angegeben werden.
4. Mehrere unabhängige Einträge mit identischem Level und identischer Priorität sollten
   nicht als stabiler Abhängigkeitsvertrag verwendet werden.

`PRE_KERNEL_1` und `PRE_KERNEL_2` laufen noch vor verfügbaren Kernel-Diensten.
`POST_KERNEL` darf Kernel-Primitiven verwenden. `APPLICATION` läuft unmittelbar vor
`main()`.

Vergleiche nun die statische ELF-Auswertung mit dem Ereignispuffer der Laufzeitausgabe.
Die Bitmaske weist lediglich nach, dass alle Hooks gelaufen sind. Erst die gespeicherten
Positionen unterscheiden beispielsweise `EARLY → PRE_KERNEL_1` von der umgekehrten
Reihenfolge. Die Init-Funktionen verwenden bewusst kein `printk()`: Insbesondere bei
`EARLY` kann die Konsole noch nicht initialisiert sein.

### 6. Ausgewählte Funktionen disassemblieren

```sh
objdump -d --demangle build/day5-5.04/zephyr/zephyr.exe | \
  grep -A 12 -E '<boot_trace_(early|pre_kernel_1|pre_kernel_2|post_kernel|application)>:'
```

Untersuche, wie die Funktionen die gemeinsame Bitmaske verändern und ihre Levelkennung
an `boot_trace_record()` übergeben. Die Aufgabe besteht nicht in einer vollständigen
Assembleranalyse, sondern in der Zuordnung von Quelltext, Symbol und erzeugtem Code.

### 7. Bootpfad dokumentieren

Erstelle aus den Beobachtungen eine kurze Ablaufskizze mit diesen Ebenen:

```text
Hoststart von native_sim
        |
        v
Zephyr-Architekturstart und z_cstart()
        |
        v
EARLY -> PRE_KERNEL_1 -> PRE_KERNEL_2 -> POST_KERNEL -> APPLICATION
        |
        v
main()
```

Markiere in der Skizze, welche Aussagen direkt aus dem Build nachgewiesen werden und
welche Teile eines realen MCU-Bootpfads unter `native_sim` nicht vorhanden sind.

## Kompakte Auswertung

Das Hilfsskript fasst die wichtigsten Artefakte zusammen. Es ersetzt nicht die einzelnen
Analyseschritte.

```sh
training-zephyr/practices/day5/5.04-boot-path/scripts/inspect-artifacts.sh \
  build/day5-5.04
```

## Hinweise zur Interpretation

- Die Reihenfolge der Symboladressen ist keine Ausführungsreihenfolge.
- `zephyr.elf` ist bei `native_sim` ein relocatable Zwischenartefakt; `zephyr.exe` ist
  das endgültige ausführbare Hostprogramm.
- `.bss` belegt Laufzeitspeicher, obwohl die Nullwerte nicht als Nutzdaten im ELF liegen.
- `SYS_INIT()` legt Level und Priorität fest; die Linker- und Generatorschritte erzeugen
  daraus die endgültige Initialisierungsreihenfolge.
- Die Bitmaske zeigt Vollständigkeit, der Ereignispuffer die Reihenfolge und das
  `initlevels`-Target die statisch im ELF hinterlegte Gesamtreihenfolge.
- Direkte Konsolenausgaben aus `EARLY`- oder `PRE_KERNEL_*`-Hooks sind kein robuster
  Nachweis, weil die benötigte Konsole zu diesem Zeitpunkt noch fehlen kann.
- `native_sim` zeigt Zephyr-Initialisierung und Anwendungseinstieg, aber keine SoC-Boot-ROM,
  keinen MCUboot-Slotwechsel und keinen physischen Resetvektor.
- Optimierung und Link-Time-Optimierung können zusätzliche lokale Symbole verändern oder
  zusammenfassen. Die ausdrücklich benannten `boot_trace_*`-Funktionen bleiben das
  stabile Analysegerüst dieser Übung.

