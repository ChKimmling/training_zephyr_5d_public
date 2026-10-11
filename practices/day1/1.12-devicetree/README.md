# 1.12 – Devicetree in native_sim

## Ziel

Eine simulierte Gerätebeschreibung im Overlay anpassen und zur Compile-Time im C-Code
auswerten.

## Ausgangssituation

Ein benutzerdefinierter Devicetree-Knoten beschreibt Name, Intervall und Grenzwert einer rein
simulierten Anwendung. Er steuert keine echte Peripherie.

## Aufgabe

1. Untersuche `app.overlay`, die Binding-Datei und den Alias `training_device`.
2. Lies Intervall, Namen und Grenzwert mit `DT_ALIAS()` und `DT_PROP()` aus.
3. Baue und starte die Anwendung.
4. Ändere Overlay-Eigenschaften und prüfe die neue Startausgabe.

## Vorgehen

```sh
west build -p always -b native_sim training-zephyr/practices/day1/1.12-devicetree/start \
  -d build/day1-1.12
./build/day1-1.12/zephyr/zephyr.exe
```

Die Lösung ist als Referenz unter `solution/` enthalten. Overlay-Änderungen werden beim
nächsten Build ausgewertet.

## Erwartetes Ergebnis

Die Lösung gibt `device=day1-sim interval=250 ms limit=4` aus. Die Werte stammen aus dem
Overlay; es findet kein Zugriff auf reale Hardware statt.
Nach der Beobachtung den laufenden `native_sim`-Prozess mit Ctrl+C beenden.
