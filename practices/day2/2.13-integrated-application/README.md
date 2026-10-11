# 2.13 – Praxis 4: Integrierte Mini-Anwendung

## Ziel

Eine simulierte Sensordaten-Anwendung aus Timer, Workqueue, Message Queue, Consumer und
Statistik zusammensetzen und ihre Invarianten überprüfen.

## Ausgangssituation

`start/` enthält ein baubares Grundgerüst. Verbinde die periodische Erfassung mit der
aufgeschobenen Arbeit und übertrage Datensätze an einen separaten Consumer.

## Aufgabe

1. Weise Timer, Workqueue, Queue, Consumer und Statistik jeweils einen Ausführungskontext und
   eine Verantwortung zu.
2. Halte den Timer-Handler kurz und nicht blockierend; übergib die Verarbeitung an eine
   Workqueue.
3. Übertrage Messwerte über eine Message Queue mit endlichem Timeout.
4. Begründe Thread-Prioritäten, Queue-Kapazität und Überlaststrategie.
5. Schütze gemeinsam veränderte Statistikwerte oder verwende passende atomare Operationen.
6. Nutze die eingebaute dreisekündige Erfassungspause alle 30 Sekunden, um den
   Empfangs-Timeout sichtbar zu prüfen; mache auch Queue-Überlast sichtbar.
7. Führe die Anwendung mindestens 30 Sekunden aus und prüfe:
   - Der Timer-Handler blockiert nicht.
   - Queue-Verluste und Timeouts werden gezählt.
   - Die Queue-Füllung überschreitet ihre Kapazität nicht.
   - Der Consumer bleibt ausführbar; kein Thread verhungert dauerhaft.
8. Dokumentiere die Nachrichtenbilanz und begründe deine Architekturentscheidungen.

## Vorgehen

```sh
west build -p always -b native_sim training-zephyr/practices/day2/2.13-integrated-application/start \
  -d build/day2-2.13
./build/day2-2.13/zephyr/zephyr.exe
```

Nach den Änderungen erneut bauen. Die Referenz lässt sich separat bauen:

```sh
west build -p always -b native_sim training-zephyr/practices/day2/2.13-integrated-application/solution \
  -d build/day2-2.13-solution
./build/day2-2.13-solution/zephyr/zephyr.exe
```

Beende den laufenden Prozess nach der Beobachtung mit Ctrl+C.

## Prüfkriterien

- Der Timer-Handler führt keine blockierende oder umfangreiche Verarbeitung aus.
- Datenübergabe erfolgt über die Message Queue.
- Full- und Timeout-Pfade sind sichtbar und bilanziert.
- Gemeinsame Statistikdaten sind gegen konkurrierende Zugriffe geschützt.
- Queue-Belegung und Nachrichtenbilanz sind nachvollziehbar.
- Die Anwendung bleibt während des Tests reaktionsfähig.

## Verwendete Zephyr-APIs

`K_TIMER_DEFINE`, `k_timer_start`, `K_WORK_DEFINE`, `k_work_submit`, `K_MSGQ_DEFINE`,
`k_msgq_put`, `k_msgq_get`, `k_msgq_num_used_get`, `K_THREAD_DEFINE`, atomare Operationen
