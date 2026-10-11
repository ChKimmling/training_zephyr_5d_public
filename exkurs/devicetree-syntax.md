# Devicetree-Syntax

Ein Devicetree (DTS) beschreibt **Hardware** als Baum aus Knoten und Eigenschaften. Zephyr
liest ihn beim Build, prüft ihn gegen **Bindings** (YAML) und erzeugt daraus C-Makros
(`devicetree_generated.h`), die im Code mit `DT_*` ausgewertet werden.

## Dateitypen

| Datei | Aufgabe |
|---|---|
| `*.dts` | Vollständige Board-Beschreibung |
| `*.dtsi` | Include-Datei (SoC, gemeinsame Teile) |
| `*.overlay` | Anwendungsspezifische Änderungen/Ergänzungen (z. B. `app.overlay`) |
| `dts/bindings/*.yaml` | Bindings: erlaubte Eigenschaften und Typen pro `compatible` |

## Grundaufbau

```dts
/ {                                   /* Wurzelknoten */
	aliases {
		training-device = &training_device;   /* Alias auf Node-Label */
	};

	training_device: training-device {    /* label: node-name */
		compatible = "training,simulated-device";
		status = "okay";
		device-name = "day1-sim";
		interval-ms = <250>;
		limit = <4>;
	};
};
```

- **Knoten:** `[label:] name[@unit-address] { ... };`
- **Eigenschaft:** `name = wert;` oder nur `name;` (boolesch).
- Jede Anweisung endet mit `;`.
- Kommentare: `/* ... */` und `// ...`.

## Wertetypen

| Syntax | Typ | Beispiel |
|---|---|---|
| `"text"` | string | `device-name = "day1-sim";` |
| `<1 2 3>` | Array aus 32-Bit-Zahlen | `interval-ms = <250>;` |
| `[00 ff]` | Byte-Array | `local-mac-address = [00 11 22 33 44 55];` |
| `<&label>` | Referenz (phandle) | `gpios = <&gpio0 13 GPIO_ACTIVE_LOW>;` |
| *(leer)* | boolean | `wakeup-source;` |
| `"a", "b"` | String-Liste | `compatible = "vendor,chip", "generic";` |

Zahlen können dezimal oder hexadezimal (`0x10`) angegeben werden; `#include` und Makros
(z. B. `GPIO_ACTIVE_LOW`) funktionieren, weil Dateien durch den C-Präprozessor laufen.

## Wichtige Eigenschaften

| Eigenschaft | Bedeutung |
|---|---|
| `compatible` | Wählt das Binding und den Treiber |
| `status` | `"okay"` aktiviert, `"disabled"` deaktiviert den Knoten |
| `reg` | Adresse/Größe, abhängig von `#address-cells`/`#size-cells` des Elternknotens |
| `label` | Menschenlesbarer Name (ältere Variante, vorrangig für Treiber-Lookup) |

## Namensregeln: Devicetree → C

Im C-Code werden Namen normalisiert: Bindestriche, Kommas, Punkte werden zu `_`, alles klein.

| DTS | C |
|---|---|
| Alias `training-device` | `DT_ALIAS(training_device)` |
| Eigenschaft `interval-ms` | `DT_PROP(node, interval_ms)` |
| Label `training_device` | `DT_NODELABEL(training_device)` |

```c
#define DEVICE_NODE DT_ALIAS(training_device)
int interval = DT_PROP(DEVICE_NODE, interval_ms);
```

## Aliases und Chosen

Beide Konstrukte geben Knoten einen **festen, boardunabhängigen Namen**, über den Code und
Zephyr-Subsysteme Hardware finden, ohne den konkreten Knoten zu kennen.

| | `aliases` | `chosen` |
|---|---|---|
| Zweck | Kurzname für einen Knoten, den die **Anwendung** nutzt (z. B. `led0`) | Legt fest, welchen Knoten ein **System-/Subsystem** verwendet (z. B. Konsole) |
| Wert | Pfad bzw. Referenz auf einen Knoten | Referenz auf einen Knoten, ggf. mit Parametern |
| Zugriff in C | `DT_ALIAS(led0)` | `DT_CHOSEN(zephyr_console)` |
| Typischer Einsatz | Portable Beispiele (`led0`, `sw0`) | `zephyr,console`, `zephyr,shell-uart`, `zephyr,sram`, `zephyr,flash` |

```dts
/ {
	aliases {
		training-device = &training_device;
	};

	chosen {
		zephyr,console = &uart0;
		zephyr,shell-uart = &uart0;
	};
};
```

```c
#define DEVICE_NODE  DT_ALIAS(training_device)
#define CONSOLE_NODE DT_CHOSEN(zephyr_console)
```

- **Alias:** Eine Anwendung greift über `DT_ALIAS(led0)` auf „die LED“ zu; jedes Board ordnet
  `led0` einer anderen realen LED zu. Der Code bleibt unverändert.
- **Chosen:** Zephyr selbst liest diese Einträge, z. B. um die Konsole auf `uart0` zu legen.
  Ein Overlay kann das ändern: `chosen { zephyr,console = &uart1; };`.
- Bei `chosen` heißen die Eigenschaften `vendor,name`; in C wird daraus
  `zephyr_console`. Bei Aliases wird `-` zu `_` (`training-device` → `training_device`).

## Overlays

Ein Overlay **ergänzt oder ändert** Knoten per Label-Referenz, ohne das Board-DTS zu ändern:

```dts
&uart0 {
	status = "okay";
	current-speed = <115200>;
};
```

Eigenschaften werden überschrieben, neue Knoten hinzugefügt. Mit `/delete-node/` und
`/delete-property/` lässt sich Vorhandenes entfernen.

## Praxis

- Zephyr findet `app.overlay` im Anwendungsordner automatisch; weitere Overlays per
  `-DEXTRA_DTC_OVERLAY_FILE=...`.
- Ergebnis prüfen: `build/<dir>/zephyr/zephyr.dts` (zusammengeführter Baum).
- Nach Overlay-Änderungen neu bauen; Fehler in Eigenschaften meldet der Build anhand des Bindings.
- Das Binding muss zum `compatible` passen und alle `required`-Eigenschaften enthalten.

## Referenz

- Zephyr-Dokumentation: [Devicetree](https://docs.zephyrproject.org/latest/build/dts/index.html)
- Zephyr-Dokumentation: [Introduction to devicetree](https://docs.zephyrproject.org/latest/build/dts/intro.html)
- Zephyr-Dokumentation: [Devicetree bindings](https://docs.zephyrproject.org/latest/build/dts/bindings.html)
- Zephyr-Dokumentation: [Devicetree API (DT_* Makros)](https://docs.zephyrproject.org/latest/build/dts/api/api.html)
- Devicetree-Spezifikation: <https://www.devicetree.org/specifications/>
