# 4.11 – TCP als vorbereiteter Telemetrie-Transport

## Lernziel

Den Vertrag eines vorbereiteten TCP-Transports verstehen: Listener und Sitzung besitzen, Antworten vollständig senden und Deskriptoren kontrolliert schließen.

## Aufgabe

`reference/` ist ein Walkthrough ohne Teilnehmer-TODOs. `include/transport.h`
definiert die Schnittstelle; `src/transport_tcp.c` kapselt Socketoperationen.
Ein interner Loopback-Client verbindet sich auf Port 4242 und sendet `GET\n`.
Die Antwort ist ein vorbereiteter Text, noch kein Sensor-Snapshot.

Untersuche `send_all()`, `recv() == 0`, Listener-/Sitzungsbesitz und
`transport_tcp_close()`. TAP und ein externer Hostclient sind nicht erforderlich.

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.11-prepared-tcp-transport/reference -d build/day4-4.11
./build/day4-4.11/zephyr/zephyr.exe
```

## Abnahmesequenz

1. Referenz bauen und starten.
2. Empfangene Antwort `seq=7 quality=OK connected=1` prüfen.
3. Die Referenz vergleicht den empfangenen Text und meldet `TCP_REFERENCE PASS`.
4. Im Code den Cleanup nach Erfolg, Peer-Close und Protokollfehler verfolgen.
5. Erklären, warum `send_all()` mehrfach senden kann und ein geschlossenes Handle
   auf −1 gesetzt wird.

## Erfolgskriterien

- Listener und angenommene Sitzung sind als getrennte Ressourcen erkennbar.
- Antwort und vorbereiteter Text stimmen im Referenzlauf überein.
- Teilweises Senden wird durch eine Schleife behandelt.
- Peer-Close wird als Fehler zurückgegeben; die Sitzung wird geschlossen.
- Cleanup kann für ein bereits geschlossenes Handle erneut aufgerufen werden.

## Hinweise und typische Fehler

- Der aktuelle Requestpfad erwartet `GET\n` in einem einzigen `recv()`.
  Auf mehrere Empfangsvorgänge verteilte Requests werden noch nicht zusammengesetzt.
- Socket-Timeouts fehlen; der Happy Path ist kein Nachweis für alle Fehlerfälle.
- Loopback-Sockets gehören zum simulierten Zephyr-Netzwerk; ein Hostprozess kann dieses
  Setup ohne zusätzliche Netzwerkanbindung nicht wie einen Hostserver ansprechen.
- Diese Grenzen bei der Integration in 4.12 berücksichtigen.
