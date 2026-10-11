# Logging vs. printk

Zephyr bietet zwei Wege für Textausgaben: das einfache `printk()` und das Logging-Subsystem
(`LOG_INF()`, `LOG_ERR()` usw.). Beide schreiben typischerweise auf die Konsole, unterscheiden
sich aber stark in Funktionsumfang und Overhead.

## Kurzvergleich

| | `printk()` | Logging-Subsystem |
|---|---|---|
| Header | `<zephyr/sys/printk.h>` | `<zephyr/logging/log.h>` |
| Kconfig | `CONFIG_PRINTK` (Standard: `y`), keine Konfiguration nötig | `CONFIG_LOG=y` |
| Log-Level | nein | `ERR`, `WRN`, `INF`, `DBG` |
| Modul-/Quellenname | nein | ja (`LOG_MODULE_REGISTER`) |
| Zeitstempel | nein | optional |
| Filterung | nein | pro Modul, zur Compile- und Laufzeit |
| Verarbeitung | synchron, direkt | wahlweise deferred (eigener Thread) oder immediate |
| Backends | Konsole | UART, RTT, Dateisystem, Netzwerk u. a. |
| Overhead (Flash/RAM) | sehr gering | deutlich höher |

## Vorteile und Nachteile

**`printk()`**

- (+) Minimaler Code- und RAM-Bedarf, keine Konfiguration nötig.
- (+) Sehr früh im Boot und in Fehlerpfaden nutzbar (z. B. Fatal Error).
- (+) Einfaches Verhalten: Ausgabe sofort, Reihenfolge eindeutig.
- (−) Kein Level, kein Modulbezug, keine Zeitstempel.
- (−) Nicht abschaltbar nach Kategorie; in Produktion nur global über Konsole.
- (−) Synchrone Ausgabe blockiert den aufrufenden Kontext (relevant in zeitkritischem Code).

**Logging**

- (+) Level und Module lassen sich gezielt ein- und ausschalten, auch per Kconfig ohne
  Code-Änderung.
- (+) Deferred-Modus verlagert die Ausgabe aus zeitkritischen Pfaden in einen Thread.
- (+) Mehrere Backends, Zeitstempel, Hexdumps (`LOG_HEXDUMP_INF`).
- (+) Nicht benötigte Level werden zur Compile-Zeit entfernt.
- (−) Mehr Flash/RAM und mehr Konfigurationsaufwand.
- (−) Im Deferred-Modus kann Ausgabe verzögert sein oder bei Überlauf Meldungen verlieren;
  bei einem Absturz sind letzte Meldungen unter Umständen noch nicht ausgegeben.

## Praktischer Einsatz

**printk:** erste „Hello World“-Tests, kleine Beispiele wie die Übungen unter `practices/`,
sehr frühe Boot-Phase, minimale Systeme ohne großen Speicher.

**Logging:** reale Anwendungen und Treiber, Fehlerdiagnose mit Levels, Ausgabe über mehrere
Kanäle (z. B. RTT während der Entwicklung), Komponenten mit mehreren Modulen.

### Beispiel

```c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
	LOG_INF("Start, interval=%d ms", 250);
	LOG_DBG("wird bei LOG_LEVEL_INF nicht ausgegeben");
	LOG_ERR("Fehlercode %d", -5);
	return 0;
}
```

```ini
# prj.conf
CONFIG_LOG=y
CONFIG_LOG_MODE_IMMEDIATE=y   # optional: ohne Verzögerung, ideal beim Debuggen
```

Wird `LOG_MODULE_REGISTER(app)` ohne Level angegeben, gilt `CONFIG_LOG_DEFAULT_LEVEL`
(z. B. `CONFIG_LOG_DEFAULT_LEVEL=4` für Debug). Eigene Kconfig-Optionen pro Modul sind
möglich, erfordern aber ein zusätzliches Kconfig-Template.

## Faustregel

`printk()` für kleine Demos und Notfälle, Logging für alles, was gewartet, gefiltert und
gezielt abgeschaltet werden soll. Nicht beide für denselben Zweck mischen: Ausgabe über
`printk()` umgeht Level und Filter des Logging-Subsystems.
