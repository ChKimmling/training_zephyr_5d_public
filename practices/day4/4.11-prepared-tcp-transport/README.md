# 4.11 – TCP als vorbereiteter Telemetrie-Transport

Das Referenzprojekt kapselt Listener, Annahme einer Sitzung, minimales `GET\n`-Framing,
Teilübertragungen und idempotentes Cleanup. Ein interner Loopback-Client macht die Demo unter
`native_sim` reproduzierbar; ein TAP-Interface ist nicht erforderlich.

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.11-prepared-tcp-transport/reference \
  -d build/day4-4.11
./build/day4-4.11/zephyr/zephyr.exe
```

Im Walkthrough sind besonders `send_all()`, `recv() == 0`, der Besitz von Listener und
Sitzungsdeskriptor sowie `transport_tcp_close()` zu prüfen. Das Modul ist vorbereitet und
enthält deshalb keine Teilnehmer-TODOs.
