# 4.07 – Ztest, Twister und Fehler-Injektion

Der Produktionscode entspricht 4.06. Im Startstand fehlen nur die fünf Testaufgaben.

| TODO | Testziel | Nachweis in der Lösung |
|---|---|---|
| `TODO(4.07-01)` | Fixture zurücksetzen | Before-Hook leert Queue und Zustand |
| `TODO(4.07-02)` | gültig und Bereichsgrenze | fachliche Assertions |
| `TODO(4.07-03)` | Sensorfehler | Qualität und genullte Achsen |
| `TODO(4.07-04)` | Queue-Überlast | zwei Drops, neueste Sequenz 6 |
| `TODO(4.07-05)` | Sequenzlücke | Sequenzen 1 und 4 ergeben zwei Lücken |

```sh
west twister -T training-zephyr/practices/day4/4.07-probe-tests/solution \
  -p native_sim --inline-logs
```

Die Tests verwenden keine fragile Logreihenfolge. Bearbeitungszeit: etwa 30 Minuten.

