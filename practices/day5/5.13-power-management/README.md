# 5.13 – Device Runtime Power Management mit `native_sim`

## Lernziel

Ein Gerät nur während seiner tatsächlichen Nutzung aktiv halten. Dazu die Zustände
`ACTIVE` und `SUSPENDED`, die Runtime-PM-Referenzzählung sowie die Busy-Markierung
gezielt einsetzen und von System Power Management unterscheiden.

## Einordnung

Die Anwendung registriert einen kleinen Trainingssensor als reguläres Zephyr-Gerät.
Sein Power-Management-Callback macht jeden Suspend- und Resume-Übergang sichtbar.
Der Treiber greift nicht auf reale Hardware zu; dadurch lässt sich der API-Ablauf mit
`native_sim` reproduzierbar untersuchen.

`native_sim` belegt dabei weder eine reale Stromaufnahme noch die Eintritts- und
Aufwachlatenzen eines Mikrocontrollers. Die Übung demonstriert die Softwarezustände
und die Verantwortlichkeiten der Anwendung. Messwerte für ein Produkt müssen später
auf dem Zielboard bestimmt werden.

## Aufgabe

Vervollständige die fünf markierten Stellen im Projekt `start/`:

| Markierung | Ort | Inhalt |
|---|---|---|
| `TODO(5.13-01)` | `training_sensor_pm_action()` | Suspend und Resume im Treiber behandeln |
| `TODO(5.13-02)` | `runtime_pm_enable()` | Runtime Power Management für das Gerät aktivieren |
| `TODO(5.13-03)` | `sensor_acquire()` | vor einem Zugriff eine Nutzungsreferenz beziehen |
| `TODO(5.13-04)` | `sensor_release()` | die Nutzungsreferenz nach dem Zugriff freigeben |
| `TODO(5.13-05)` | `critical_operation()` | Gerät während einer kritischen Operation als busy markieren |

Die Dateien und der Programmablauf von `start/` und `solution/` stimmen überein. Die
Musterlösung ersetzt ausschließlich die TODO-Implementierungen.

## Vorgehen

### 1. Ausgangszustand beobachten

Baue und starte zunächst den unvollständigen Stand:

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day5/5.13-power-management/start \
  -d build/day5-5.13-start
./build/day5-5.13-start/zephyr/zephyr.exe
```

Ordne die ausgegebenen Rückgabewerte den noch fehlenden Implementierungen zu. Der
Sensor startet aktiv, weil sein Treiber initialisiert wurde. Erst das Aktivieren von
Runtime PM überführt ihn bei einer Nutzungszahl von null in den suspendierten Zustand.

### 2. Power-Management-Callback ergänzen

Implementiere in `training_sensor.c` die Aktionen:

- `PM_DEVICE_ACTION_SUSPEND`: Sensor deaktivieren,
- `PM_DEVICE_ACTION_RESUME`: Sensor aktivieren,
- andere Aktionen: mit `-ENOTSUP` ablehnen.

Erhöhe den Übergangszähler nur bei einer unterstützten Aktion. Der Callback gehört zum
Treiber: Nur er kennt die hardwarenahe Reihenfolge, die ein reales Gerät zum Abschalten
oder Reaktivieren benötigt.

### 3. Runtime PM aktivieren

Rufe in `runtime_pm_enable()` die passende Zephyr-API auf. Beobachte anschließend den
Wechsel von `ACTIVE` nach `SUSPENDED`. `pm_device_runtime_enable()` schaltet nicht eine
globale Energiesparstufe ein, sondern aktiviert die nutzungsbasierte Verwaltung dieses
einen Geräts.

### 4. Nutzung korrekt klammern

Ergänze `sensor_acquire()` und `sensor_release()` mit den synchronen Runtime-PM-APIs.
Verfolge danach die Zustände und den Usage Counter:

```text
get:  0 -> 1  RESUME
get:  1 -> 2  kein weiterer Zustandswechsel
put:  2 -> 1  Gerät bleibt ACTIVE
put:  1 -> 0  SUSPEND
```

Der Zähler beschreibt Nutzer, nicht Gerätezugriffe. Jeder erfolgreiche `get` benötigt
genau ein zugehöriges `put`. Untersuche auch den Rückgabewert des absichtlich
unbalancierten letzten `put`.

### 5. Kritische Operation markieren

Setze in `critical_operation()` vor dem simulierten Flashzugriff die Busy-Markierung
und entferne sie auf jedem Rückkehrpfad wieder. Die Markierung informiert das
systemverwaltete Device PM darüber, dass das Gerät nicht suspendiert werden darf. Sie
ersetzt keine Runtime-PM-Referenz und hält den Usage Counter deshalb unverändert.

### 6. Lösung bauen und Ausgabe erklären

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day5/5.13-power-management/solution \
  -d build/day5-5.13-solution
./build/day5-5.13-solution/zephyr/zephyr.exe
```

Erkläre für jede `SNAPSHOT`-Zeile:

- welcher Anwendungsschritt vorausging,
- warum sich Zustand oder Usage Counter geändert haben,
- ob der Treiber-Callback ausgeführt wurde,
- warum `busy=1` keine zusätzliche Nutzungsreferenz erzeugt.

### 7. Device PM und System PM abgrenzen

Übertrage die Beobachtungen auf ein reales Zielsystem:

| Mechanismus | Entscheidung | Typischer Auslöser |
|---|---|---|
| Device Runtime PM | einzelnes Gerät aktiv oder suspendiert | `pm_device_runtime_get()` und `put()` |
| System-managed Device PM | Geräte vor einem Systemzustand behandeln | System-PM-Ablauf und Busy-Status |
| System Power Management | CPU- oder SoC-Zustand auswählen | Idle-Fenster, Policy und Constraints |

Bewerte für eine reale Plattform zusätzlich Mindestresidenzzeit und Exit-Latenz. Ein
tiefer Zustand ist nur sinnvoll, wenn das freie Zeitfenster die Eintritts-,
Mindestaufenthalts- und Aufwachkosten trägt und kein aktiver Constraint ihn sperrt.
Die funktionale Ausgabe dieser Übung liefert dafür keine Zeit- oder Energiemessung.

## Konfiguration und Symbole untersuchen

```sh
grep -E 'CONFIG_PM_DEVICE(=|_RUNTIME=)' \
  build/day5-5.13-solution/zephyr/.config
nm build/day5-5.13-solution/zephyr/zephyr.exe | \
  grep -E 'pm_device_runtime_(enable|get|put)|training_sensor_pm_action'
```

Ordne die aktivierten Konfigurationsoptionen den verwendeten APIs zu. Prüfe außerdem,
welche Funktionen im fertigen Hostprozess als Symbole erhalten bleiben.

## Hinweise

- Ein fehlgeschlagenes `get` darf nicht durch ein `put` ausgeglichen werden.
- Fehler nach einem erfolgreichen `get` benötigen dennoch einen gemeinsamen
  Aufräumpfad.
- Asynchrones `put` kann Wartezeit vermeiden, erweitert aber den Zustandsraum um
  `SUSPENDING` und erfordert eine passende Workqueue-Konfiguration.
- Abhängigkeiten und Power Domains müssen auf Hardware gemeinsam betrachtet werden.
- Während Flash- und Updateoperationen müssen benötigte Geräte, Takte und
  Speicherbereiche aktiv bleiben.
