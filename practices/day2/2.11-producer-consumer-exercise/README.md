# 2.11 – Praxis 3: Producer–Consumer-Pipeline

## Ziel

Simulierte Messwerte zwischen Threads über eine Message Queue übertragen und Überlast
beobachtbar sowie kontrolliert behandeln.

## Ausgangssituation

Das Gerüst enthält eine Producer- und Consumer-Struktur sowie Platzhalter für Queue,
Nachrichtenbilanz und Überlaststrategie. Der Producer soll schneller Werte erzeugen als der
Consumer sie verarbeitet.

## Aufgabe

1. Definiere einen Datensatz mit Sequenznummer, simuliertem Messwert und Erzeugungszeit.
2. Definiere eine `k_msgq` mit begründeter Kapazität.
3. Vervollständige Producer und Consumer und werte Rückgabewerte der Queue-Operationen aus.
4. Verwende eine endliche Empfangszeit und behandle einen Timeout explizit.
5. Mache den Producer schneller als den Consumer und beobachte die volle Queue.
6. Implementiere eine Überlaststrategie, zum Beispiel Verwerfen, Warten oder Anpassen der
   Produktionsrate.
7. Gib am Ende eine konsistente Bilanz aus: erzeugt, angenommen, verarbeitet, verworfen,
   Timeouts und maximale Queue-Füllung.

## Vorgehen

```sh
west build -p always -b native_sim training-zephyr/practices/day2/2.11-producer-consumer-exercise/start \
  -d build/day2-2.11
./build/day2-2.11/zephyr/zephyr.exe
```

Das Gerüst ist ein Startpunkt; ergänze die markierten Stellen und baue nach jeder Etappe
erneut. Die Referenz lässt sich separat bauen:

```sh
west build -p always -b native_sim training-zephyr/practices/day2/2.11-producer-consumer-exercise/solution \
  -d build/day2-2.11-solution
./build/day2-2.11-solution/zephyr/zephyr.exe
```

## Prüfkriterien

- Die Anwendung endet nach einer endlichen Zahl von Nachrichten.
- Erzeugte Nachrichten entsprechen angenommenen und verworfenen Nachrichten.
- Angenommene Nachrichten werden vollständig verarbeitet.
- Volle Queue, Timeouts und Queue-Füllstand sind sichtbar.
- Es gibt keine stillschweigend überschriebenen Nutzdaten.

## Verwendete Zephyr-APIs

`K_MSGQ_DEFINE`, `k_msgq_put`, `k_msgq_get`, `k_msgq_num_used_get`, `K_THREAD_DEFINE`,
`k_sem_take`
