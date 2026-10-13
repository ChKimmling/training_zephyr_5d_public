# 3.12 – Interaktiver Event Service

## Lernziel

Shell, GPIO-Stimulus, Callback, asynchrone Verarbeitung, Status und Statistik zu einer
reproduzierbaren Event-Anwendung integrieren.

## Aufgabe

`start/` und `solution/` enthalten dieselbe Projektstruktur, Devicetree-Belegung, Shell-
Oberfläche und öffentlichen Header. Das Teilnehmerprojekt baut und startet; die TODOs
markieren die zu vervollständigenden Teile. Implementiere insbesondere Stimulusadapter und
Status, vervollständige die Callback-/Queue-Integration und prüfe Fehler- und Resetverhalten.
Der GPIO-Callback bleibt kurz und blockiert nicht. Die Shell greift ausschließlich über
öffentliche APIs auf Service und Stimulus zu.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.12-event-service/start \
  -d build/day3-3.12
./build/day3-3.12/zephyr/zephyr.exe
```

Für die Referenz baue separat:

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.12-event-service/solution \
  -d build/day3-3.12-solution
./build/day3-3.12-solution/zephyr/zephyr.exe
```

## Abnahmeszenarien

Starte mit einem frischen Build und führe die Szenarien nacheinander aus. Warte nach
asynchronen Sequenzen mit `event status` oder `event stats`, bis `stimulus=idle` gilt und der
Worker die Queue abgearbeitet hat.

### Startzustand und einzelner Puls

```text
event status
event stats
event pulse 100
event status
event stats
```

Erwartet werden `initialized=yes`, `input_ready=yes`, `stimulus=idle` und zunächst null
Zähler. Während des Pulses zeigt `event status` `stimulus=pulse`; nach Ende ist der Pegel
inaktiv. Beide Pegelflanken werden gezählt: ein vollständig abgearbeiteter Puls erzeugt
`raw=2`, `processed=2`.

### Burst und parallele Bedienung

```text
event burst 5 200
event status
event stats
event pulse 100
```

Der Burst wird nicht blockierend angenommen; Status und Statistik bleiben währenddessen
abrufbar. Ein Puls während des Bursts wird mit Busy-Fehler abgewiesen. Nach Abschluss erzeugt
der Fünfer-Burst zehn Flanken, sofern keine Queue-Überläufe aufgetreten sind.

### Ungültige Eingaben und Statistik-Reset

```text
event input 2
event pulse 0
event burst 999999 1
event status
event stats
event reset
event stats
```

Jede vom Handler ausgewertete ungültige Anforderung wird verständlich abgewiesen und erhöht
`rejected`; sie darf keinen GPIO-Stimulus starten. Shell-Fehler bei einer falschen Anzahl
von Argumenten werden bereits vor dem Handler abgewiesen und daher nicht mitgezählt.
`event reset` setzt `raw`, `processed`, `rejected` und `lost` zurück, ändert aber weder
Gerätebereitschaft noch Eingangspegel oder Sequenzzustand.

### Queue-Überlast

```text
event reset
event burst 20 20
event status
event stats
```

Der Burst-Abstand von 20 ms wird nach dem Ende jedes 100-ms-Pulses gemessen. Der Event-Worker
simuliert absichtlich 250 ms Verarbeitung pro GPIO-Flanke; so wird die Message Queue mit
Kapazität 8 überlastbar, während Callback und Shell reaktionsfähig bleiben.

`raw` zählt jede steigende und fallende GPIO-Flanke. `processed` zählt vom Worker
abgearbeitete Flanken, `lost` nicht einreihbare Queue-Nachrichten und `rejected` ungültige
oder gerade nicht zulässige Bedienanforderungen. Während Verarbeitung kann `processed`
vorübergehend hinter `raw` liegen. Warte nach Ende des Bursts, bis die Queue abgearbeitet ist;
dann gilt `raw = processed + lost`.

Der Statistik-Reset erhöht intern eine Epochennummer. Bereits vor dem Reset eingereihte
Nachrichten werden deshalb nach dem Reset nicht in die neuen Zähler übernommen. Geräte- und
Stimuluszustand bleiben unberührt.

## Fehler- und Zustandsregeln

- `event input` akzeptiert ausschließlich `0` und `1` und ist während `pulse`/`burst` nicht
  zulässig.
- Pulsdauer: 20–2000 ms, Standard: 100 ms.
- Burst-Anzahl: 1–20 Pulse; Abstand: 20–2000 ms zwischen Pulsende und nächstem Puls.
- Ein weiterer Stimulus während einer Sequenz wird mit `-EBUSY` abgewiesen.
- `event cancel` bricht eine laufende Sequenz ab und stellt den inaktiven Pegel her.
- Callback-Ausgaben werden nicht direkt auf der Shell erzeugt; interne Diagnose gehört ins
  Logging, Bedienantworten in die Shell.

## Erfolgskriterien

- Anwendung und Shell funktionieren vollständig unter `native_sim`.
- Callback bleibt kurz und nicht blockierend; die Queue erhält Ereignisse außerhalb davon.
- Status und Statistik sind während einer Sequenz abrufbar.
- Invalid-, Busy- und Queue-voll-Fälle werden sichtbar, ohne den Service zu destabilisieren.
- Die Anwendung endet nach Puls/Burst im definierten Ruhezustand.
- Nach dem Leeren der Queue gilt `raw = processed + lost`.
- Ein Reset setzt nur Statistiken zurück und lässt Geräte- und Stimuluszustand unverändert.
