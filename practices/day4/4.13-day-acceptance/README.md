# 4.13 – Integrierte Tests, Diagnose und Tagesabnahme

Der Ordner `reference/` ist der qualifizierte Tagesendstand und kein weiteres
Start-/Lösungspaar. Das Abnahmeskript baut ihn frisch, prüft zwei TCP-Sitzungen und startet
die Ztest-/Twister-Suite aus 4.07.

```sh
sh training-zephyr/practices/day4/4.13-day-acceptance/scripts/check.sh .
```

Manuell sind zusätzlich Devicetree, `.config`, Sensorfehler, Queue-Policy,
Connectivity-Transitionen, Deskriptor-Cleanup und der fortschreitende Sensorzustand während
Disconnect zu erklären. Erwartete Invarianten stehen in `fixtures/expected.txt`.

Dieser Referenzstand ist die Übergabe an Tag 5. Optionale Devkit- und BLE-Dateien sind nicht
Teil der Abnahme.

