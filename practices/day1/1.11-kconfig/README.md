# 1.11 – Konfiguration mit Kconfig

## Ziel

Laufzeitverhalten über Kconfig-Werte variieren, ohne den C-Code zwischen Builds zu ändern.

## Ausgangssituation

Die Anwendung besitzt Kconfig-Symbole für Grenzwert, Intervall, Diagnoseausgabe und aktiven
Startzustand. Die Referenz enthält zwei `prj.conf`-Varianten.

## Aufgabe

1. Untersuche die Deklarationen in `Kconfig` und Standardwerte.
2. Ordne Werte in `prj.conf` den `CONFIG_...`-Makros im C-Code zu.
3. Baue und starte die Standardvariante.
4. Baue mit `verbose.conf` als Zusatzkonfiguration neu.
5. Vergleiche Grenzwert, Ausgabeintervall und Diagnoseausgabe.

## Vorgehen

Standardvariante:

```sh
west build -p always -b native_sim training-zephyr/practices/day1/1.11-kconfig/solution \
  -d build/day1-1.11
./build/day1-1.11/zephyr/zephyr.exe
```

Alternative Variante mit `EXTRA_CONF_FILE`:

```sh
west build -p always -b native_sim training-zephyr/practices/day1/1.11-kconfig/solution \
  -d build/day1-1.11-verbose -- -DEXTRA_CONF_FILE=verbose.conf
./build/day1-1.11-verbose/zephyr/zephyr.exe
```

Für die Teilnehmeraufgabe zunächst `start/` als Anwendung verwenden. Das Beispiel bleibt nach
Grenzwert-Erreichung aktiv; mit Ctrl+C beenden.

## Erwartetes Ergebnis

Standardwerte sind Grenzwert 4, Intervall 250 ms und Startzustand `STARTUP`. Mit
`verbose.conf` wird der Grenzwert 2, das Intervall 100 ms, die Diagnoseausgabe aktiviert und
der initiale Zustand auf `ACTIVE` gesetzt.
