# 4.09 – Network Management und Connection Manager

## Lernziel

Network Management und Connection Manager unterscheiden und den Weg eines L4-Ereignisses vom Callback in ein Work Item nachvollziehen.

## Aufgabe

`demo/` ist vollständig und enthält keine Teilnehmer-TODOs. Lies `src/main.c` und
ordne Callback, atomaren Zwischenzustand und Work Item ihren Aufgaben zu.
`conn_mgr_mon_resend_status()` fordert nach der Registrierung den aktuellen Monitorstatus
an. Die Demo verwendet Loopback und benötigt kein TAP-Setup.

Die manuell ausgelösten Connect-/Disconnect-Events sind Lehrstimuli; sie ändern den
realen Interface-Zustand nicht.

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.09-network-management-walkthrough/demo -d build/day4-4.09
./build/day4-4.09/zephyr/zephyr.exe
```

## Abnahmesequenz

1. Demo bauen und starten.
2. Meldungen `callback context` und `worker context` identifizieren.
3. Eventname, Interface und übernommenen Zustand erklären; der Pointerwert ist kein Sollwert.
4. Unterschied zwischen aktuellem Status und späterem Zustandswechsel erläutern.
5. `NET_MGMT_DEMO PASS` zusammen mit den beobachteten Events bewerten.

Die PASS-Marke wird nach dem Auslösen der Events ausgegeben und prüft keine Eventbilanz.
Netzwerkevents können asynchron eintreffen; `k_work_flush()` wartet nur auf das Work Item,
nicht auf einen noch ausstehenden Netzwerk-Callback. Eine starre Logreihenfolge oder eine
bestimmte Zahl von Worker-Aufrufen ist daher kein verlässliches Abnahmekriterium.

## Erfolgskriterien

- Interface, Eventverteilung und aggregierter L4-Zustand werden unterschieden.
- Callback und nachgelagerte Verarbeitung sind im Code lokalisierbar.
- Die Anwendung fragt den initialen Monitorstatus nach der Registrierung erneut an.
- Die Grenzen der synthetischen Lehrstimuli sind erklärbar.

## Hinweise und typische Fehler

- `L4_CONNECTED` bestätigt keinen erfolgreichen TCP-Verbindungsaufbau zu einem Server.
- Eventmasken ersetzen keine genaue Prüfung des Eventcodes im Callback.
- Zusätzliche Monitorereignisse und zusammengefasste Work Items bei der Diagnose beachten.

Vertiefung: [Network Management und Connection Manager](../../../exkurs/network-manager.md).
