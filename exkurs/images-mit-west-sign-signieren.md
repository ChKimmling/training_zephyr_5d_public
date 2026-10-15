# Exkurs: Zephyr-Images mit `west sign` und `imgtool` signieren

Ein signiertes Firmware-Image erlaubt dem Bootloader zu prüfen, ob Inhalt und Herkunft
des Images vertrauenswürdig sind. Bei Zephyr-Anwendungen mit MCUboot erzeugt `imgtool`
den MCUboot-Header, die kryptografischen TLVs und die Signatur. `west sign` dient als
Zephyr-Wrapper: Es findet die Buildartefakte, ermittelt soweit möglich Parameter aus dem
Build und ruft das ausgewählte Signierwerkzeug auf.

Dieser Exkurs verwendet Zephyr 4.4.2 und den im Workspace enthaltenen MCUboot-Stand.
Die Befehle werden aus dem Workspace-Root `training-zephyr-ws/` ausgeführt.

## Signatur, Hash und Verschlüsselung unterscheiden

| Mechanismus | Aussage | Schützt nicht vor |
|---|---|---|
| SHA-Hash | Der Inhalt besitzt einen bestimmten Fingerabdruck. | Austausch von Image und Hash durch einen Angreifer |
| Digitale Signatur | Das Image wurde mit dem passenden privaten Schlüssel signiert und danach nicht verändert. | Einsicht in den unverschlüsselten Imageinhalt |
| Transportverschlüsselung | Die Übertragung ist gegen Mitlesen oder Manipulation geschützt. | Manipulation eines ungeschützt gespeicherten Images nach der Übertragung |
| Imageverschlüsselung | Der gespeicherte Imageinhalt ist nicht unmittelbar lesbar. | Ausführung eines authentischen, aber verwundbaren alten Images |
| Security Counter | Ein signiertes Image unterschreitet nicht die erlaubte Sicherheitsgeneration. | Fachlich falsche, aber neuere und korrekt signierte Firmware |

Signieren und Verschlüsseln lösen unterschiedliche Probleme. Ein signiertes Image ist
nicht automatisch geheim, und eine verschlüsselte Datei ist ohne Authentisierung nicht
automatisch vertrauenswürdig.

## Beteiligte Werkzeuge

| Werkzeug | Aufgabe | Typische Verwendung |
|---|---|---|
| `west build` | Anwendung bauen und Build-Metadaten erzeugen | `zephyr.bin`, `zephyr.hex`, Devicetree und `.config` erstellen |
| `west sign` | Buildverzeichnis und externes Signierwerkzeug verbinden | vorhandenes Zephyr-Artefakt nachträglich signieren |
| `imgtool.py` | MCUboot-Images erzeugen, signieren und untersuchen | `keygen`, `getpub`, `sign`, `verify`, `dumpinfo` |
| `sha256sum` | vollständige Dateien vergleichen | unbeabsichtigte Änderungen und unterschiedliche Artefakte erkennen |
| `stat` und `ls` | Dateigröße und Rechte prüfen | Schutz des privaten Schlüssels und Padding kontrollieren |
| `xxd` oder `hexdump` | ausgewählte Bytes darstellen | Magic, Headeranfang oder Trailerposition nachvollziehen |

Die verfügbaren Optionen des lokal installierten Stands zeigen diese Befehle:

```sh
west sign -h
python bootloader/mcuboot/scripts/imgtool.py -h
python bootloader/mcuboot/scripts/imgtool.py sign -h
python bootloader/mcuboot/scripts/imgtool.py dumpinfo -h
```

## Voraussetzungen

Aktiviere das Virtual Environment und wähle für `native_sim` bei Bedarf die
Host-Toolchain:

```sh
source .venv/bin/activate
export ZEPHYR_TOOLCHAIN_VARIANT=host
```

Das MCUboot-Projekt muss im West-Workspace vorhanden sein:

```sh
west list mcuboot
test -f bootloader/mcuboot/scripts/imgtool.py
```

Die Python-Abhängigkeiten stammen im Trainingsworkspace aus Zephyrs Requirements. Falls
`imgtool.py` wegen eines fehlenden Python-Moduls nicht startet, müssen zuerst die zum
Workspace gehörenden Requirements installiert werden. Ein beliebiges globales
`imgtool` kann eine andere Version besitzen und sollte nicht unbemerkt mit dem
Workspace vermischt werden.

## Beispielprojekt bauen

Übung 5.06 liefert ein kleines Projekt und erzeugt ausdrücklich ein BIN-Artefakt:

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day5/5.06-sign-image/reference \
  -d build/day5-5.06
```

Prüfe die Ausgangsartefakte:

```sh
ls -lh build/day5-5.06/zephyr/zephyr.exe \
  build/day5-5.06/zephyr/zephyr.elf \
  build/day5-5.06/zephyr/zephyr.bin
sha256sum build/day5-5.06/zephyr/zephyr.bin
```

Für diesen `native_sim`-Build ist `zephyr.exe` das ausführbare Hostprogramm.
`zephyr.bin` dient nur als kontrollierte Nutzlast für die Format- und Signierübung. Das
Signieren macht die BIN-Datei nicht zu einem startbaren Hostprogramm.

## Trainingsschlüssel mit imgtool erzeugen

Erzeuge Schlüssel niemals direkt im Quellverzeichnis. Für die Übung wird ein eigener
Artefaktordner unter `build/` verwendet:

```sh
mkdir -p build/day5-5.06/artifacts
umask 077
python bootloader/mcuboot/scripts/imgtool.py keygen \
  --key build/day5-5.06/artifacts/training-ec-p256.pem \
  --type ecdsa-p256
```

Leite anschließend den öffentlichen Schlüssel ab:

```sh
python bootloader/mcuboot/scripts/imgtool.py getpub \
  --key build/day5-5.06/artifacts/training-ec-p256.pem \
  --encoding pem \
  --output build/day5-5.06/artifacts/training-ec-p256-public.pem
chmod 600 build/day5-5.06/artifacts/training-ec-p256.pem
chmod 644 build/day5-5.06/artifacts/training-ec-p256-public.pem
stat -c '%a %n' build/day5-5.06/artifacts/training-ec-p256*.pem
```

Alternativ kapselt das vorhandene Trainingsskript dieselben Schritte und verweigert das
Überschreiben eines vorhandenen Schlüssels:

```sh
training-zephyr/practices/day5/5.06-sign-image/scripts/prepare-training-key.sh \
  build/day5-5.06/artifacts
```

Der private Schlüssel darf weder in Git noch in Buildlogs, Tickets oder Terminalmitschnitte
gelangen. Der öffentliche Schlüssel beziehungsweise dessen Hash wird so in MCUboot
eingebunden, dass der Bootloader nur Images aus der zugehörigen Vertrauenskette
akzeptiert.

## `west sign` verstehen

Der allgemeine Aufbau lautet:

```text
west sign [West-Optionen] -t <Werkzeug> [-p <Werkzeugpfad>] -- [Werkzeugoptionen]
```

Alles vor `--` wird von `west sign` verarbeitet. Alles danach wird an das ausgewählte
Werkzeug weitergegeben. Der Trenner sollte auch dann verwendet werden, wenn ein Aufruf
zufällig ohne ihn funktioniert; er verhindert Mehrdeutigkeiten zwischen West- und
Werkzeugoptionen.

Wichtige West-Optionen:

| Option | Bedeutung |
|---|---|
| `-d`, `--build-dir` | zu verwendendes Zephyr-Buildverzeichnis |
| `-t`, `--tool` | Signierwerkzeug, hier `imgtool` |
| `-p`, `--tool-path` | expliziter Pfad zum Werkzeug |
| `--bin`, `--no-bin` | signierte BIN-Datei erzeugen oder auslassen |
| `--hex`, `--no-hex` | signierte HEX-Datei erzeugen oder auslassen |
| `-B`, `--sbin` | abweichender Name für das signierte BIN-Artefakt |
| `-H`, `--shex` | abweichender Name für das signierte HEX-Artefakt |
| `-f`, `--force` | erkannte Voraussetzungen ignorieren und den Versuch fortsetzen |

`-f` bedeutet nicht nur „vorhandene Datei überschreiben“. Es lässt `west sign` trotz
erkannter Probleme fortfahren und sollte deshalb nicht unkritisch in einem
Produktionsskript stehen.

## Manuell mit `west sign` signieren

Der Trainingsbuild für `native_sim` besitzt weder MCUboot-Partitionen noch eine passende
Bootloaderkonfiguration. Deshalb werden Layoutparameter ausdrücklich angegeben und
`west sign` wird nur für diese Formatübung mit `-f` fortgesetzt:

```sh
west sign -f -d build/day5-5.06 \
  -t imgtool \
  -p bootloader/mcuboot/scripts/imgtool.py \
  --bin --no-hex -- \
  --key build/day5-5.06/artifacts/training-ec-p256.pem \
  --version 1.2.3+4 \
  --header-size 0x200 \
  --slot-size 0x69000 \
  --align 4 \
  --pad-header
```

Das Ergebnis liegt standardmäßig hier:

```text
build/day5-5.06/zephyr/zephyr.signed.bin
```

Die vier zentralen Layoutparameter bedeuten:

| Parameter | Bedeutung |
|---|---|
| `--version` | Version im MCUboot-Imageheader |
| `--header-size` | reservierter Bereich vor der Nutzlast |
| `--slot-size` | verfügbare Größe des Zielslots einschließlich Trailerreserve |
| `--align` | vom gewählten Swapverfahren benötigte Schreibausrichtung |

`--pad-header` fügt den reservierten Headerbereich vor der Nutzlast ein. `--pad` ist
davon verschieden: Es erweitert das fertige Image bis zur Slotgröße und erzeugt den
Trailerbereich. Für einen Test- oder bereits bestätigten Zustand können abhängig vom
gewählten Updateverfahren `--test` oder `--confirm` verwendet werden; beide implizieren
Padding bis zur Slotgröße.

Diese Werte dürfen für ein reales Gerät nicht aus dem Trainingsbeispiel übernommen
werden. Dort müssen Headergröße, Slotgrenzen, Alignment und Updateverfahren exakt mit
Devicetree, MCUboot-Build und Flashgeometrie übereinstimmen.

## Von west sign ausgeführtes Kommando sichtbar machen

Führe den Aufruf ohne `-q` aus. `west sign` zeigt das aufgerufene Werkzeug und dessen
Parameter. Für eine reproduzierbare Diagnose sollten außerdem Buildkonfiguration und
Partitionsdaten festgehalten werden:

```sh
grep -E 'CONFIG_(BOOTLOADER_MCUBOOT|MCUBOOT|BUILD_OUTPUT)' \
  build/day5-5.06/zephyr/.config
grep -n -E 'slot0_partition|slot1_partition' \
  build/day5-5.06/zephyr/zephyr.dts || true
```

Fehlen die Partitionen bei `native_sim`, ist das in dieser Übung erwartet. Auf einem
realen Ziel ist es dagegen ein Hinweis, dass Build und Speicherlayout nicht für den
gewünschten MCUboot-Pfad vorbereitet sind.

## Signiertes Image untersuchen

`dumpinfo` dekodiert Header, TLVs und – falls vorhanden – Trailer:

```sh
python bootloader/mcuboot/scripts/imgtool.py dumpinfo \
  build/day5-5.06/zephyr/zephyr.signed.bin
```

Für maschinenlesbare Ausgaben stehen YAML und JSON zur Verfügung:

```sh
python bootloader/mcuboot/scripts/imgtool.py dumpinfo \
  --format json \
  --outfile build/day5-5.06/artifacts/zephyr.signed.json \
  build/day5-5.06/zephyr/zephyr.signed.bin
```

Zu prüfen sind insbesondere:

- MCUboot-Magic und Headergröße,
- Nutzlastgröße und Imageversion,
- Hash-TLV,
- Key-Hash oder öffentlicher Schlüssel gemäß Formatwahl,
- Signatur-TLV,
- Security Counter, sofern verwendet,
- Trailerzustand bei gepaddeten Images.

Ein Dateihash über das komplette signierte Image ist zusätzlich nützlich, ersetzt aber
keine Signaturprüfung:

```sh
sha256sum build/day5-5.06/zephyr/zephyr.bin \
  build/day5-5.06/zephyr/zephyr.signed.bin
```

## Signatur mit dem öffentlichen Schlüssel prüfen

Zur Verifikation wird der private Schlüssel nicht benötigt:

```sh
python bootloader/mcuboot/scripts/imgtool.py verify \
  --key build/day5-5.06/artifacts/training-ec-p256-public.pem \
  build/day5-5.06/zephyr/zephyr.signed.bin
```

Prüfe auch den Fehlerfall mit einem anderen öffentlichen Schlüssel. Ein erfolgreich
signiertes Image ist nur für einen Bootloader verwendbar, dessen eingebauter
Vertrauensanker zu genau diesem Signierschlüssel passt.

## Manipulation nach dem Signieren erkennen

Die Übung 5.06 enthält ein Werkzeug, das genau ein Bit in der Nutzlast ändert:

```sh
training-zephyr/practices/day5/5.06-sign-image/scripts/tamper-image.py \
  build/day5-5.06/zephyr/zephyr.signed.bin \
  build/day5-5.06/artifacts/zephyr.tampered.bin
python bootloader/mcuboot/scripts/imgtool.py verify \
  --key build/day5-5.06/artifacts/training-ec-p256-public.pem \
  build/day5-5.06/artifacts/zephyr.tampered.bin
```

Die Verifikation muss fehlschlagen: Die veränderte Nutzlast erzeugt einen anderen Hash,
der nicht mehr zur gespeicherten Signatur passt. Der erwartete Fehler ist Teil der
Analyse und darf in einem Shellskript nicht versehentlich durch ein unkontrolliertes
`|| true` verborgen werden.

## Signieren im regulären MCUboot-Sysbuild

Für ein reales Ziel ist der integrierte Buildpfad vorzuziehen. Sysbuild baut MCUboot und
Anwendung gemeinsam, propagiert Speicherlayout und Signaturtyp und erzeugt passende
signierte Artefakte:

```sh
west build -p always -b <board> <anwendung> \
  --sysbuild -d build/signed -- \
  -DSB_CONFIG_BOOTLOADER_MCUBOOT=y \
  -DSB_CONFIG_BOOT_SIGNATURE_TYPE_ECDSA_P256=y \
  -DSB_CONFIG_BOOT_SIGNATURE_KEY_FILE=/absoluter/pfad/release-ec-p256.pem
```

Anschließend werden die tatsächlich übernommenen Werte geprüft:

```sh
grep -E 'SB_CONFIG_(BOOTLOADER_MCUBOOT|BOOT_SIGNATURE)' \
  build/signed/zephyr/.config build/signed/_sysbuild/.config 2>/dev/null
grep -E 'CONFIG_BOOT_SIGNATURE_TYPE|CONFIG_BOOT_SIGNATURE_KEY_FILE' \
  build/signed/mcuboot/zephyr/.config
find build/signed -type f \( -name '*.signed.bin' -o -name '*.signed.hex' \) -print
```

Die genauen Konfigurationspfade können sich je nach Zephyr-Version und Buildaufbau
unterscheiden. Entscheidend ist, nicht nur die Kommandozeile, sondern die erzeugten
Konfigurationen und Artefakte zu kontrollieren.

Für zusätzliche `imgtool`-Optionen im automatischen Buildpfad dient
`CONFIG_MCUBOOT_EXTRA_IMGTOOL_ARGS`. Ein bestätigt erzeugtes Produktionsimage kann über
`CONFIG_MCUBOOT_GENERATE_CONFIRMED_IMAGE` angefordert werden. Ob ein bestätigtes Image
fachlich korrekt ist, hängt jedoch vom Provisionierungs- und Updateprozess ab.

## Hinweis zur Deprecation von west sign mit imgtool

Im verwendeten Zephyr 4.4.2 ist der `imgtool`-Teil von `west sign` als veraltet
gekennzeichnet. Seit Zephyr 4.0 ruft der automatische MCUboot-Build `west sign` nicht
mehr als Zwischenschicht auf. Stattdessen wird die Signierung direkt durch CMake und die
MCUboot-Integration ausgeführt.

Daraus folgt:

- `west sign -t imgtool` bleibt für die manuelle Analyse bestehender Builds nützlich.
- Neue Produktbuilds sollten die Sysbuild-/CMake-Signierung verwenden.
- Zusätzliche automatische Optionen gehören in
  `CONFIG_MCUBOOT_EXTRA_IMGTOOL_ARGS`, nicht in entfernte ältere West-Konfigurationen.
- Build- und Releaseautomation darf nicht von einer bereits als veraltet markierten
  Schnittstelle abhängig gemacht werden, ohne einen Migrationsplan zu besitzen.

## Schlüsselverwaltung für Produktion

Der Beispielschlüssel aus diesem Exkurs ist ausschließlich ein Trainingsschlüssel. Für
Produktionsschlüssel gelten mindestens diese Regeln:

- Schlüssel nicht im Repository oder normalen CI-Dateisystem speichern.
- Signierrechte von Buildrechten trennen.
- Zugriff protokollieren und auf autorisierte Releaseprozesse begrenzen.
- Offline-Signierung, HSM oder verwalteten Signing Service prüfen.
- Schlüsselrotation und Widerruf vor dem ersten Release planen.
- Entwicklungs-, Test- und Produktionsvertrauensanker strikt trennen.
- Öffentliche Schlüssel beziehungsweise Hashes reproduzierbar in MCUboot provisionieren.
- Releaseartefakt, Quellrevision, Konfiguration, Signatur und Security Counter gemeinsam
  dokumentieren.

`imgtool` unterstützt mit `--vector-to-sign`, `--sig-out`, `--fix-sig` und
`--fix-sig-pubkey` auch Bausteine für externes Signieren. Der konkrete Ablauf muss aber
an das verwendete HSM oder den Signing Service angepasst und als zusammenhängender
Releaseprozess validiert werden.

## Häufige Fehlerbilder

| Symptom | Wahrscheinliche Ursache | Prüfung |
|---|---|---|
| `west sign` findet kein BIN | `CONFIG_BUILD_OUTPUT_BIN` fehlt oder falsches Buildverzeichnis | `.config` und `build/.../zephyr/zephyr.bin` prüfen |
| Header- oder Slotgröße fehlt | Build besitzt keine auswertbare MCUboot-Partitionierung | Devicetree und MCUboot-/Sysbuild-Konfiguration prüfen |
| Image ist größer als der Slot | Nutzlast, Header und TLVs überschreiten die nutzbare Slotgröße | `dumpinfo`, Map-Datei und Partitionsgrößen vergleichen |
| MCUboot lehnt das Image ab | Schlüssel, Algorithmus, Header, Alignment oder Trailer passen nicht | MCUboot-Konfiguration und `dumpinfo` gemeinsam prüfen |
| `verify` schlägt mit öffentlichem Schlüssel fehl | falsches Schlüsselpaar oder nachträglich verändertes Image | Schlüsselhash, Dateihash und Signatur-TLV vergleichen |
| Image startet, fällt aber zurück | Testimage wurde nicht rechtzeitig bestätigt | Selbstprüfung und `boot_write_img_confirmed()` untersuchen |
| Signiertes BIN ist nicht ausführbar | BIN ist ein Flashimage, kein Hostprogramm | für `native_sim` weiterhin `zephyr.exe` starten |
| Signierte Dateien sind nicht bytegleich | probabilistische Signatur oder unterschiedliche Metadaten | fachliche Felder und kryptografische Verifikation vergleichen |

## Kompakte Befehlsfolge für die Übung

```sh
source .venv/bin/activate
export ZEPHYR_TOOLCHAIN_VARIANT=host

west build -p always -b native_sim \
  training-zephyr/practices/day5/5.06-sign-image/reference \
  -d build/day5-5.06

training-zephyr/practices/day5/5.06-sign-image/scripts/prepare-training-key.sh \
  build/day5-5.06/artifacts

west sign -f -d build/day5-5.06 \
  -t imgtool \
  -p bootloader/mcuboot/scripts/imgtool.py \
  --bin --no-hex -- \
  --key build/day5-5.06/artifacts/training-ec-p256.pem \
  --version 1.2.3+4 \
  --header-size 0x200 \
  --slot-size 0x69000 \
  --align 4 \
  --pad-header

python bootloader/mcuboot/scripts/imgtool.py dumpinfo \
  build/day5-5.06/zephyr/zephyr.signed.bin
python bootloader/mcuboot/scripts/imgtool.py verify \
  --key build/day5-5.06/artifacts/training-ec-p256-public.pem \
  build/day5-5.06/zephyr/zephyr.signed.bin
```

## Bezug zu Übungen und weiteren Exkursen

- [5.06 – Zephyr-Image signieren und prüfen](../practices/day5/5.06-sign-image/README.md)
  stellt das verwendete Projekt und die Hilfsskripte bereit.
- [5.10 – Update, Bestätigung und Revert](../practices/day5/5.10-update-state-analysis/README.md)
  ordnet die Signatur in Slot- und Trailerzustände ein.
- [Firmware-Updateverfahren](update-verfahren.md) vergleicht Swap, Overwrite,
  Direct-XIP, RAM-Load und Recovery-Verfahren.
- [west Meta-Tool und Manifest](west-und-manifest.md) erklärt Workspace und
  West-Erweiterungsbefehle.

## Weiterführende Primärquellen

- [Zephyr: Signing Binaries](https://docs.zephyrproject.org/latest/build/signing/index.html)
- [Zephyr: west sign](https://docs.zephyrproject.org/latest/develop/west/sign.html)
- [MCUboot: imgtool](https://docs.mcuboot.com/imgtool.html)
- [MCUboot: Signed Images](https://docs.mcuboot.com/signed_images.html)
