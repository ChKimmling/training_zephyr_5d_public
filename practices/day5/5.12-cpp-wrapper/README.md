# 5.12 – C++-Wrapper für eine C-Schnittstelle

## Lernziel

Eine bestehende C-Schnittstelle durch einen kleinen C++-Typ kapseln, Besitz und
Lebensdauer sichtbar machen und C-Fehlercodes in starke Ergebnistypen übersetzen.
Die Implementierung ohne Exceptions, RTTI oder dynamische Allokation in eine
`native_sim`-Anwendung integrieren und ihre Build-Artefakte untersuchen.

## Aufgabe

`common/` enthält die unveränderte C-Schnittstelle und ein deterministisches
Trainings-Backend. `start/` und `solution/` besitzen dieselbe öffentliche
C++-Schnittstelle, dieselbe Anwendung und dieselbe Build-Konfiguration. Ergänze nur
die nummerierten TODOs in `start/src/sensor.cpp`.

Der Startstand ist eigenständig baubar. Seine Methoden melden absichtlich einen nicht
verfügbaren Wrapper, bis die C-Schnittstelle angebunden wurde. `solution/` zeigt die
zugehörige Referenzimplementierung. `c-reference/` verwendet dasselbe Backend direkt
und dient ausschließlich dem Artefaktvergleich.

| TODO | Funktion | Inhalt |
|---|---|---|
| `TODO(5.12-01)` | `Sensor::~Sensor()` | besessene Ressource am Scope-Ende freigeben |
| `TODO(5.12-02)` | `Sensor::init()` | starken Kanaltyp an die C-API übergeben und Besitz übernehmen |
| `TODO(5.12-03)` | `Sensor::init()` | C-Fehlercodes einmalig in `InitStatus` übersetzen |
| `TODO(5.12-04)` | `Sensor::read()` | Sample lesen und `ReadStatus` samt Nutzwert zurückgeben |
| `TODO(5.12-05)` | `Sensor::ready()` | gültigen Besitzstatus sichtbar machen |

## Vorgehen

### 1. C-Sprachgrenze untersuchen

Lies `common/include/sensor_api.h` und `common/src/sensor_api.c`. Ordne zu:

- warum die Deklarationen für C++ innerhalb von `extern "C"` liegen,
- welcher Typ den Backendzustand besitzt,
- welche Fehler als `-EINVAL`, `-EBUSY`, `-EACCES` und `-EIO` sichtbar werden,
- warum der Aufrufer den Speicher für Handle und Sample bereitstellt.

Die C-API bleibt während der Übung unverändert.

### 2. Besitz und Lebensdauer implementieren

`Sensor` besitzt ein `sensor_api_handle` direkt als Member. Der Typ ist nicht kopierbar
und benötigt keinen Heap. Ergänze `init()` und den Destruktor so, dass eine erfolgreich
geöffnete Ressource genau einmal geschlossen wird.

Die explizite `init()`-Methode ist hier dem Konstruktor vorgezogen, weil
`sensor_api_open()` fachlich fehlschlagen kann. Vergleiche diese Entscheidung mit einer
Factory, die Status und Objekt getrennt zurückgeben müsste.

### 3. Fehler typsicher übersetzen

Implementiere `read()` und übersetze die C-Rückgabewerte an der Sprachgrenze:

- `0` nach `ReadStatus::ok` und ein gültiges `Sample`,
- `-EBUSY` nach `ReadStatus::busy`,
- `-EACCES` nach `ReadStatus::not_ready`,
- übrige Fehler nach `ReadStatus::io_error`.

Es werden weder Exceptions ausgelöst noch negative Ganzzahlen bis nach `main.cpp`
durchgereicht.

### 4. Statische Konfiguration nachvollziehen

`SensorConfig` kombiniert `Channel` und die Anzahl der Leseversuche. Die
`constexpr`-Konfiguration in `main.cpp` wird vollständig zur Übersetzungszeit gebildet.
Erkläre, welche Verwechslungen `enum class Channel` gegenüber einem untypisierten
Ganzzahlparameter verhindert.

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day5/5.12-cpp-wrapper/start \
  -d build/day5-5.12-start
./build/day5-5.12-start/zephyr/zephyr.exe

west build -p always -b native_sim \
  training-zephyr/practices/day5/5.12-cpp-wrapper/solution \
  -d build/day5-5.12-solution
./build/day5-5.12-solution/zephyr/zephyr.exe

west build -p always -b native_sim \
  training-zephyr/practices/day5/5.12-cpp-wrapper/c-reference \
  -d build/day5-5.12-c
./build/day5-5.12-c/zephyr/zephyr.exe
```

Die Lösung zeigt erfolgreiche Messungen sowie die vorbereiteten Zustände `busy` und
`io_error`. Nach dem Ende des lokalen Scopes lässt sich dieselbe Ressource erneut
öffnen; damit wird die Freigabe durch den Destruktor sichtbar.

## Symbole und Größe untersuchen

```sh
nm -C build/day5-5.12-solution/zephyr/zephyr.exe | \
  grep -E 'training::Sensor|sensor_api_'

size build/day5-5.12-c/zephyr/zephyr.exe \
  build/day5-5.12-start/zephyr/zephyr.exe \
  build/day5-5.12-solution/zephyr/zephyr.exe
```

Ordne Konstruktor, Destruktor und Methoden anhand der demangelten Symbolnamen zu.
Vergleiche anschließend `text`, `data` und `bss`. Der Vergleich zeigt den konkreten
Host-Build dieser Übung; daraus lassen sich keine absoluten Speicherwerte für ein
Mikrocontrollerziel ableiten.

## Hinweise zur Interpretation

- `extern "C"` ändert die Sprachbindung, nicht die Laufzeitsemantik der C-Funktion.
- RAII ersetzt keine Fehlerbehandlung bei der Initialisierung. Es koppelt die Freigabe
  an die Lebensdauer einer erfolgreich übernommenen Ressource.
- Das Ergebnisobjekt enthält nur dann einen fachlich gültigen Messwert, wenn sein Status
  `ReadStatus::ok` ist.
- Der Wrapper speichert Handle und Ergebniswerte direkt; `new`, `delete` und versteckte
  Containerallokationen werden nicht benötigt.
- Globale C++-Objekte werden bewusst vermieden. Das lokale Objekt besitzt eine klar
  erkennbare Konstruktion und Destruktion innerhalb von `main()`.
