# Zephyr 5-Tage-Workshop

Willkommen im Material zum fünftägigen Zephyr-RTOS-Workshop. Das Repo wächst mit dem Training:
**an jedem Workshop-Tag kommen die neuen Folien, Übungen und Exkurse des Tages hinzu.** Mit
`git pull` im Ordner `training/` holt ihr euch jeweils den aktuellen Stand.

Die Übungen laufen überwiegend auf `native_sim`, also direkt auf dem Linux-Host, ohne
Hardware. Technisch ist das Repo ein off-tree Zephyr Workspace (West T2) auf Basis von
**Zephyr v4.4.2**: Es ist das Manifest-Repo ([west.yml](west.yml)); Zephyr und die Module werden
daneben ausgecheckt. Der Checkout ist klein gehalten: nur workshop-relevante Module,
`clone-depth: 1`, kein Import von Zephyrs eigenem `west.yml`.

## Inhalt
| Ordner | Zweck |
|---|---|
| `slides/day1..day5` | **Folien**: der rote Faden durch das Training, pro Tag ein Ordner |
| `practices/day1..day5` | **Übungen** zu den Folien des Tages, Fokus `native_sim` |
| `exkurs/` | **Exkurse**: ergänzende Detailinformationen zum Nachlesen und Vertiefen, z. B. „Portabilität: Datentypen“, „Logging vs. `printk`“ oder „Kconfig im Detail“ |

Die Exkurse sind nicht Teil des roten Fadens; die Folien verweisen an passender Stelle darauf.

## Voraussetzungen (Linux, Host)
```sh
sudo apt install --no-install-recommends git cmake ninja-build gperf ccache dfu-util \
  device-tree-compiler python3-dev python3-venv python3-pip python3-setuptools wget xz-utils \
  file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1
```
(`gcc-multilib` wird für `native_sim` benötigt.)

## Setup
### 1. Workspace-Ordner und Virtual Environment
```sh
mkdir training-zephyr-ws && cd training-zephyr-ws
python3 -m venv .venv
source .venv/bin/activate
pip install --upgrade pip
pip install west
```

### 2. Manifest-Repo klonen und Workspace initialisieren
```sh
git clone https://github.com/ChKimmling/training_zephyr_5d_private.git training
west init -l training
west update --narrow            # ca. 1 GB
west zephyr-export
```

### 3. Python-Abhängigkeiten von Zephyr
```sh
pip install -r zephyr/scripts/requirements.txt
```
Das Virtual Environment bei jeder neuen Shell mit `source .venv/bin/activate` im Ordner
`training-zephyr-ws` aktivieren.

Ergebnis:
```
training-zephyr-ws/
├── .venv/
├── training/        # dieses Repo (Manifest, Folien, Übungen)
├── zephyr/
├── modules/  bootloader/  tools/
```

## Zephyr SDK und Blobs
Im aktivierten venv, im Ordner `training-zephyr-ws`:

### Zephyr SDK (Toolchains)
v4.4 verlangt Zephyr SDK **1.0.x** (SDK 0.17.x wird nicht akzeptiert, siehe `zephyr/SDK_VERSION`).
```sh
west sdk install                                  # alle Toolchains, groß
west sdk install --toolchains arm-zephyr-eabi     # nur ARM (empfohlen für den Workshop)
```
Standard-Installationsort ist `~/zephyr-sdk-<version>`; mit `--install-dir <pfad>` änderbar.
Der Host-Compiler-Test von `native_sim` funktioniert auch ohne SDK:
```sh
export ZEPHYR_TOOLCHAIN_VARIANT=host
```
Mit installiertem SDK wird es automatisch gefunden (`west sdk list` zeigt Installationen).

### Binary Blobs
Einige Module liefern Binärdateien nur auf Anfrage aus. Verfügbare Blobs anzeigen und laden:
```sh
west blobs list
west blobs fetch                # alle Blobs der ausgecheckten Module
west blobs fetch hal_nordic     # oder gezielt pro Modul
```
Für die `native_sim`-Übungen werden keine Blobs benötigt; relevant nur für Hardware-Targets.

## Erster Test
```sh
west build -p -b native_sim zephyr/samples/hello_world -d build
./build/zephyr/zephyr.exe       # mit Ctrl+C beenden
```

## Enthaltene Projekte
| Projekt | Zweck |
|---|---|
| zephyr | Kernel, Treiber, Subsysteme |
| cmsis, cmsis_6 | CMSIS Core (Cortex-M) |
| hal_nordic | nRF-HAL (Hardware-Exkurs) |
| mbedtls, tf-psa-crypto | Crypto/TLS |
| mcuboot | Bootloader / DFU |
| littlefs, fatfs | Dateisysteme |
| zcbor | CBOR (MCUmgr/SMP) |
| picolibc | C-Bibliothek |
| net-tools | TAP-Netzwerk für `native_sim` |
| segger | RTT / SystemView |

### STM32 (optional)
`hal_stm32` (~1 GB) ist standardmäßig deaktiviert:
```sh
west config manifest.group-filter -- +hal-stm32
west update --narrow
```

## Pflege des Manifests
Zephyr-Upgrade: `revision` von `zephyr` ändern und die SHAs/Pfade der übrigen Projekte aus
`west.yml` des neuen Zephyr-Tags übernehmen (`repo-path` bei `cmsis_6` beachten).
