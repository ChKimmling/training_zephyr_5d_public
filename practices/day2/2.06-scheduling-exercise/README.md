# 2.06 – Praxis 1: Scheduling sichtbar machen

## Ziel

Scheduling-Reihenfolgen aus Thread-Prioritäten und Warteoperationen vorhersagen, beobachten
und begründen.

## Ausgangssituation

`start/` enthält drei statische Worker mit unterscheidbaren Ausgaben. Ergänze die Beobachtung
und ändere die Konfiguration, um verschiedene Scheduling-Situationen zu untersuchen.

## Aufgabe

1. Notiere vor dem Start, welcher Worker zuerst läuft und wann andere Worker dazwischenlaufen
   können.
2. Baue und starte die Anwendung. Protokolliere die Ausgaben und vergleiche sie mit deiner
   Vorhersage.
3. Ändere Prioritäten und Warteverhalten. Wiederhole für `k_sleep()` und `k_yield()`.
4. Vergleiche zwei präemptive Threads gleicher Priorität mit und ohne freiwillige Abgabe.
5. Untersuche einen kooperativen Thread gegenüber einem präemptiven Thread.
6. Erzeuge eine Starvation-Situation, erkläre ihre Ursache und behebe sie durch eine Änderung
   am Ausführungsverhalten.
7. Optional: Aktiviere `CONFIG_TIMESLICING=y` in `prj.conf` und untersuche Time-Slicing für
   präemptive Threads gleicher Priorität. Vergleiche mit freiwilliger Abgabe.

## Vorgehen

```sh
west build -p always -b native_sim training-zephyr/practices/day2/2.06-scheduling-exercise/start \
  -d build/day2-2.06
./build/day2-2.06/zephyr/zephyr.exe
```

Für weitere Durchläufe passe die Thread-Prioritäten und die Warteoperationen in
`src/main.c` an. Nach jeder Änderung erneut bauen. Beende den Prozess mit Ctrl+C.

## Prüfkriterien

- Vorhersage und Beobachtung sind für mindestens zwei Konfigurationen festgehalten.
- Jede Umschaltung lässt sich mit Priorität, Blockierung oder freiwilliger Abgabe erklären.
- Bereite und blockierte Threads werden unterschieden.
- Die Starvation wird durch eine begründete Änderung am Ausführungsverhalten behoben.

## Hinweise zur Interpretation

`native_sim` läuft auf dem Host. Hostlast und Ausgaben beeinflussen die beobachteten Zeitabstände;
die Ausgabe ist kein Hardware-Echtzeitnachweis.

## Verwendete Zephyr-APIs

`K_THREAD_DEFINE`, `k_sleep`, `k_yield`, `k_thread_name_set`
