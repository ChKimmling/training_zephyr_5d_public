# Kconfig-Syntax

Kconfig beschreibt **Konfigurationsoptionen** (Symbole) mit Typ, Standardwert und Abhängigkeiten.
Die Werte werden aus `prj.conf`, Board-Dateien und `Kconfig`-Defaults zusammengeführt und als
`CONFIG_*`-Makros im C-Code (`autoconf.h`) sowie in CMake bereitgestellt.

## Zwei Arten von Dateien

| Datei | Aufgabe | Syntax |
|---|---|---|
| `Kconfig`, `Kconfig.*` | **Definiert** Symbole (Typ, Default, Abhängigkeiten) | Kconfig-Sprache |
| `prj.conf`, `*.conf` | **Setzt** Werte für Symbole | `CONFIG_NAME=wert` |

## Werte setzen (`prj.conf`)

```ini
CONFIG_PRINTK=y            # bool: y oder n
CONFIG_MAIN_STACK_SIZE=2048 # int
CONFIG_APP_NAME="demo"     # string
CONFIG_FOO_ADDR=0x1000     # hex
# CONFIG_LOG is not set    # entspricht =n
```

- Das Präfix `CONFIG_` ist in `.conf`-Dateien Pflicht, in `Kconfig`-Dateien wird es weggelassen.
- Strings stehen in Anführungszeichen.
- Kommentare beginnen mit `#`.

## Symbole definieren (`Kconfig`)

```kconfig
config APP_INTERVAL_MS
	int "Intervall in Millisekunden"
	default 250
	range 10 5000
	depends on PRINTK
	help
	  Zeitabstand der Ausgabe in der Anwendung.
```

Wichtige Schlüsselwörter:

| Schlüsselwort | Bedeutung |
|---|---|
| `config NAME` | Definiert ein Symbol (ohne `CONFIG_`) |
| `bool`, `int`, `hex`, `string` | Typ (optional mit Prompt-Text) |
| `default WERT [if BEDINGUNG]` | Standardwert |
| `depends on AUSDRUCK` | Symbol nur sichtbar/aktivierbar, wenn Ausdruck wahr |
| `select NAME` | Erzwingt anderes Symbol auf `y` (Abhängigkeiten werden **nicht** geprüft; sparsam einsetzen) |
| `imply NAME` | Schwächeres `select`; Wert kann überschrieben werden |
| `range MIN MAX` | Zulässiger Wertebereich für `int`/`hex` |
| `help` | Beschreibungstext (eingerückt) |

## Strukturierung

```kconfig
menu "Meine Anwendung"

config APP_FEATURE
	bool "Feature aktivieren"

if APP_FEATURE
config APP_FEATURE_LEVEL
	int "Level"
	default 1
endif

endmenu

source "Kconfig.zephyr"
```

- `menu`/`endmenu` gruppieren Optionen, `if`/`endif` fügen allen Einträgen eine Abhängigkeit hinzu.
- `source "pfad"` bindet eine weitere Kconfig-Datei ein.
- In Zephyr-Anwendungen mit eigener `Kconfig` wird am Ende `source "Kconfig.zephyr"` benötigt.

## Praxis

- Ergebnis prüfen: `build/<dir>/zephyr/.config` (alle Werte) bzw. `zephyr/include/generated/zephyr/autoconf.h`.
- Interaktiv ändern: `west build -t menuconfig` (temporär; dauerhafte Änderungen in `prj.conf`).
- Zusatzdatei beim Build: `west build ... -- -DEXTRA_CONF_FILE=verbose.conf`.
- Im C-Code: `#if defined(CONFIG_FOO)` oder `IS_ENABLED(CONFIG_FOO)`; Werte direkt als `CONFIG_APP_INTERVAL_MS`.
- Wird ein Symbol in `prj.conf` gesetzt, das nicht existiert oder dessen Abhängigkeiten nicht
  erfüllt sind, ignoriert Kconfig den Wert; der Build meldet dazu eine Warnung bzw. einen Fehler.

## Referenz

- Zephyr-Dokumentation: [Kconfig](https://docs.zephyrproject.org/latest/build/kconfig/index.html)
  (Konzepte, Setzen von Werten, Tips und Tricks)
- Zephyr-Dokumentation: [Kconfig-Sprache und Erweiterungen](https://docs.zephyrproject.org/latest/build/kconfig/extensions.html)
- Zephyr-Dokumentation: [Kconfig-Suche](https://docs.zephyrproject.org/latest/kconfig.html) (alle Symbole)
- Kconfig-Sprachspezifikation (Linux Kernel): <https://docs.kernel.org/kbuild/kconfig-language.html>
