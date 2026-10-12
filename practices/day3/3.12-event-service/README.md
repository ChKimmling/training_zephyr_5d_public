# 3.12 – Interaktiver Event Service

## Lernziel

Shell, GPIO-Stimulus, Callback, asynchrone Verarbeitung, Status und Statistik zu einer
reproduzierbaren Event-Anwendung integrieren.

## Aufgabe

Integriere eine GPIO-Eingabe aus dem `native_sim`-Overlay mit der Event-Verarbeitung und
der Bedienoberfläche. Der Callback soll nur das Ereignis weitergeben; Workqueue oder Queue
erledigt die Verarbeitung außerhalb des Callback-Kontexts. Die Shell greift ausschließlich
auf öffentliche APIs zu. Vervollständige die TODOs in `start/`; `solution/` dient als
Referenz.

## Build und Start

```sh
west build -p always -b native_sim training-zephyr/practices/day3/3.12-event-service/start \
  -d build/day3-3.12
./build/day3-3.12/zephyr/zephyr.exe
```

Für die Referenz baue `solution/` nach `build/day3-3.12-solution`.

## Abnahmeszenarien

```text
event status
event stats
event pulse 100
event status
event stats
event burst 20 20
event status
event stats
event input 2
event pulse 0
event burst 999999 1
event status
event stats
event reset
event stats
```

Der Einzelpuls ist 100 ms aktiv. Der Überlasttest verwendet einen Burst-Abstand von 20 ms;
der Event-Worker simuliert absichtlich 250 ms Verarbeitung pro Ereignis. Damit wird die Queue
mit Kapazität 8 überlastet, ohne den GPIO-Callback oder die Shell zu blockieren. `raw` zählt
GPIO-Flanken, `processed` die vom Event-Worker abgearbeiteten Nachrichten, `lost`
Queue-Überläufe und `rejected` ungültige oder gerade nicht zulässige Bedienanforderungen. Da
Verarbeitung asynchron ist, kann `processed` kurzzeitig hinter `raw` liegen. Der Statistik-Reset
setzt Zähler zurück, nicht Geräteinitialisierung oder Eingangspegel. Bereits eingereihte
Nachrichten werden anhand ihres Epoch-Zählers als vor dem Reset erzeugt erkannt und nicht in
den neuen Statistikstand gezählt. Nach dem Burst weiter `event stats` abfragen, bis der
Event-Worker die Queue geleert hat; dann gilt `raw = processed + lost`.

## Erfolgskriterien

- Anwendung und Shell funktionieren vollständig unter `native_sim`.
- Callback bleibt kurz und nicht blockierend; die Queue erhält Ereignisse außerhalb davon.
- Status und Statistik sind während einer Sequenz abrufbar.
- Invalid-, Busy- und Queue-voll-Fälle werden sichtbar, ohne den Service zu destabilisieren.
- Die Anwendung endet nach Puls/Burst im definierten Ruhezustand.
