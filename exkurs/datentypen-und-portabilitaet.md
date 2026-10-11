# Datentypen und Portabilität

Zephyr-Code soll auf unterschiedlichen Architekturen laufen (8/16/32/64 Bit, Little- und
Big-Endian, `native_sim` auf dem Host oder Mikrocontroller). Die Größe vieler Standard-C-Typen
ist aber **implementierungsabhängig**. Wer sie unbedacht verwendet, erhält Code, der auf
`native_sim` (64 Bit) funktioniert und auf dem Zielboard (32 Bit) falsch läuft.

## Grundregeln

- **Typen fester Breite** aus `<stdint.h>` verwenden: `uint8_t`, `int16_t`, `uint32_t`,
  `int64_t`.
- **Größen und Indizes** mit `size_t`, Zeigerdifferenzen mit `ptrdiff_t`, Zeigerwerte als
  Ganzzahl mit `uintptr_t`.
- **`int`, `long`, `char` nur dort**, wo die Breite keine Rolle spielt (z. B. Schleifenzähler,
  Rückgabecodes). `char` ist je nach Plattform signed oder unsigned.
- **Format-Makros** aus `<inttypes.h>` verwenden (`PRIu32`, `PRId64`) statt `%d`/`%ld` zu raten.
- **Keine Annahmen** über Struct-Layout, Padding oder Endianness bei Daten, die nach außen
  gehen (Netzwerk, Flash, Bus-Protokolle).
- **Zephyr-Helfer** nutzen: `BIT()`, `ARRAY_SIZE()`, `sys_get_be16()`, `sys_cpu_to_le32()`,
  `BUILD_ASSERT()`.

## Typische Größen

| Typ | 32-Bit-MCU (z. B. Cortex-M) | 64-Bit-Host (`native_sim` 64) |
|---|---|---|
| `int` | 4 Byte | 4 Byte |
| `long` | 4 Byte | 8 Byte |
| `size_t` / Zeiger | 4 Byte | 8 Byte |
| `uint32_t` | 4 Byte | 4 Byte |

## Beispiele

**1. `long` statt fester Breite**

```c
long timestamp = k_uptime_get();   /* 32 Bit auf MCU: Überlauf nach ~25 Tagen in ms */
int64_t timestamp = k_uptime_get(); /* portabel */
```

**2. Falsches Format für `size_t`/`uint32_t`**

```c
printk("len=%d\n", sizeof(buf));              /* falsch: size_t != int */
printk("len=%zu\n", sizeof(buf));             /* richtig */
printk("val=%" PRIu32 "\n", value);           /* richtig für uint32_t */
```

**3. Zeiger in `int` speichern**

```c
int addr = (int)ptr;            /* bricht auf 64 Bit */
uintptr_t addr = (uintptr_t)ptr; /* portabel */
```

**4. Struct direkt über ein Protokoll senden**

```c
struct msg { uint8_t id; uint32_t value; };   /* Padding + Endianness plattformabhängig */
send(&msg, sizeof(msg));
```

Besser: Felder einzeln serialisieren, z. B. mit `sys_put_le32(value, &buf[1])`, oder
`__packed` plus explizite Byte-Order verwenden.

**5. Bitoperationen mit falscher Breite**

```c
uint32_t mask = 1 << 31;        /* undefiniertes Verhalten: 1 ist ein signed int */
uint32_t mask = BIT(31);        /* korrekt (unsigned) */
```

**6. Annahmen absichern**

```c
BUILD_ASSERT(sizeof(struct header) == 8, "Unerwartete Header-Größe");
```

## Prüfen

Dieselbe Anwendung für `native_sim` und ein 32-Bit-Ziel bauen und Compiler-Warnungen
(`-Wall -Wextra`, Format-Warnungen) auswerten. Unterschiede in `sizeof` und in der
Ausgabe zeigen Portabilitätsprobleme früh.
