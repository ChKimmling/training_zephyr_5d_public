# 5.06 – Zephyr-Image signieren und prüfen

## Lernziel

Ein für `native_sim` gebautes Zephyr-Binärartefakt mit einem ausschließlich für das
Training erzeugten Schlüssel signieren. Header, Nutzlast, TLVs und Trailer voneinander
unterscheiden sowie eine nachträgliche Veränderung kryptografisch erkennen.

## Einordnung

`reference/` ist ein vollständiges Projekt ohne Teilnehmer-TODOs. Es erzeugt neben dem
ausführbaren Simulator `zephyr.exe` bewusst auch `zephyr.bin` als kompakte Nutzlast für
die Formatübung. Die Signatur macht dieses BIN-Artefakt nicht auf dem Host ausführbar.

Die Übung besitzt kein `start`-/`solution`-Paar: Bearbeitet werden Signierparameter und
separat erzeugte Artefakte, nicht der Anwendungsquelltext. Der private Trainingsschlüssel
wird erst im Build-Verzeichnis erzeugt und gehört weder in Git noch in ein Release.

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day5/5.06-sign-image/reference \
  -d build/day5-5.06
./build/day5-5.06/zephyr/zephyr.exe
```

Die Anwendung meldet die fachliche Imageversion `1.2.3+4`. Alle folgenden Schritte
arbeiten mit Kopien oder neuen Dateien; `zephyr.bin` bleibt unverändert.

## Aufgabe

### 1. Ausgangsartefakte einordnen

```sh
ls -lh build/day5-5.06/zephyr/zephyr.exe \
  build/day5-5.06/zephyr/zephyr.elf \
  build/day5-5.06/zephyr/zephyr.bin
sha256sum build/day5-5.06/zephyr/zephyr.bin
```

Ordne den ausführbaren Hostprozess, das relocatable Zephyr-Artefakt und die für diese
Übung verwendete rohe Nutzlast ein. Notiere den SHA-256-Wert des Ausgangsimages.

### 2. Trainingsschlüssel erzeugen

```sh
training-zephyr/practices/day5/5.06-sign-image/scripts/prepare-training-key.sh \
  build/day5-5.06/artifacts
```

Das Skript erzeugt ein ECDSA-P-256-Schlüsselpaar:

- `training-ec-p256.pem` enthält den privaten Schlüssel und erhält Modus `0600`.
- `training-ec-p256-public.pem` enthält nur den öffentlichen Schlüssel.

Prüfe Dateirechte und Dateigrößen. Zeige den privaten Schlüssel nicht im Terminal an
und kopiere ihn nicht aus dem Trainingsverzeichnis.

### 3. Image direkt mit imgtool signieren

```sh
training-zephyr/practices/day5/5.06-sign-image/scripts/imgtool.sh sign \
  --key build/day5-5.06/artifacts/training-ec-p256.pem \
  --version 1.2.3+4 \
  --header-size 0x200 \
  --slot-size 0x69000 \
  --align 4 \
  --pad-header \
  build/day5-5.06/zephyr/zephyr.bin \
  build/day5-5.06/artifacts/app-signed.bin
```

Ordne die vier Parameter Version, Headergröße, Slotgröße und Alignment dem Imageformat
beziehungsweise dem späteren Flashlayout zu. `--pad-header` reserviert den Header vor
der Nutzlast; es füllt noch nicht den gesamten Slot.

### 4. Gepaddete Variante erzeugen

Wiederhole den vorherigen Aufruf mit `--pad` und schreibe das Ergebnis nach
`build/day5-5.06/artifacts/app-signed-padded.bin`.

```sh
training-zephyr/practices/day5/5.06-sign-image/scripts/imgtool.sh sign \
  --key build/day5-5.06/artifacts/training-ec-p256.pem \
  --version 1.2.3+4 \
  --header-size 0x200 \
  --slot-size 0x69000 \
  --align 4 \
  --pad-header --pad \
  build/day5-5.06/zephyr/zephyr.bin \
  build/day5-5.06/artifacts/app-signed-padded.bin
```

Vergleiche die Dateigrößen. Das ungepaddete Image endet hinter den TLVs; die gepaddete
Variante wird bis zur Slotgröße erweitert und enthält einen Image-Trailer.

### 5. Dasselbe Image über west sign erzeugen

```sh
west sign -f -d build/day5-5.06 \
  -t imgtool \
  -p bootloader/mcuboot/scripts/imgtool.py -- \
  --key build/day5-5.06/artifacts/training-ec-p256.pem \
  --version 1.2.3+4 \
  --header-size 0x200 \
  --slot-size 0x69000 \
  --align 4 \
  --pad-header
cp build/day5-5.06/zephyr/zephyr.signed.bin \
  build/day5-5.06/artifacts/app-west-signed.bin
```

`west sign` übernimmt das Ausgangs-BIN und Partitionsinformationen aus dem Build. Für
`native_sim` ist `CONFIG_BOOTLOADER_MCUBOOT` absichtlich nicht aktivierbar; deshalb sind
`-f` und die expliziten Imageparameter nötig. Die Warnung dokumentiert diese Grenze der
Formatübung. Auf einem realen MCUboot-Ziel stammen die Parameter aus dessen Build- und
Flashkonfiguration. Der Hinweis, dass die imgtool-Integration von `west sign` als
veraltet markiert ist, stammt aus der aktuell verwendeten Zephyr-Version.

### 6. Header und TLVs untersuchen

```sh
training-zephyr/practices/day5/5.06-sign-image/scripts/imgtool.sh dumpinfo \
  build/day5-5.06/artifacts/app-signed.bin
training-zephyr/practices/day5/5.06-sign-image/scripts/imgtool.sh dumpinfo \
  build/day5-5.06/artifacts/app-signed-padded.bin
```

Lokalisiere und erläutere:

- Magic, Headergröße, Imagegröße und Version,
- SHA-256-TLV, Key-Hash und ECDSA-Signatur,
- den Trailer der gepaddeten Variante.

Die beiden Signierwege müssen dieselben fachlichen Parameter tragen und erfolgreich
prüfbar sein. ECDSA-Signaturbytes und damit die kompletten Dateien müssen jedoch nicht
bytegleich sein.

### 7. Signaturen prüfen

```sh
for image_file in \
  build/day5-5.06/artifacts/app-signed.bin \
  build/day5-5.06/artifacts/app-signed-padded.bin \
  build/day5-5.06/artifacts/app-west-signed.bin
do
  training-zephyr/practices/day5/5.06-sign-image/scripts/imgtool.sh verify \
    --key build/day5-5.06/artifacts/training-ec-p256-public.pem \
    "$image_file"
done
```

Unterscheide die Rollen: Der private Schlüssel erzeugt die Signatur; der öffentliche
Schlüssel reicht für die Prüfung aus.

### 8. Veränderung der Nutzlast nachweisen

```sh
training-zephyr/practices/day5/5.06-sign-image/scripts/tamper-image.py \
  build/day5-5.06/artifacts/app-signed.bin \
  build/day5-5.06/artifacts/app-tampered.bin
training-zephyr/practices/day5/5.06-sign-image/scripts/imgtool.sh verify \
  --key build/day5-5.06/artifacts/training-ec-p256-public.pem \
  build/day5-5.06/artifacts/app-tampered.bin
```

Der zweite Befehl endet erwartungsgemäß mit einem Fehler. Erkläre die Kette von der
geänderten Nutzlast über den abweichenden Hash bis zur nicht mehr gültigen Signatur.
Vergleiche abschließend erneut den Hash von `zephyr.bin`: Das Ausgangsimage wurde nicht
verändert.

## Hinweise zur Interpretation

- Signieren schützt Authentizität und Integrität, verschlüsselt die Nutzlast aber nicht.
- Header- und Slotgröße sind Eigenschaften des Zielsystems, keine frei wählbaren Werte
  für ein späteres Produktimage.
- `native_sim` stellt hier Build-Artefakt und Werkzeuge bereit, simuliert aber weder eine
  Boot-ROM noch die Verifikation durch einen tatsächlich gestarteten MCUboot.
- Der private Trainingsschlüssel ist absichtlich kurzlebig. Produktionsschlüssel gehören
  in einen getrennten, zugriffsbeschränkten Signierprozess.
- Ein unsigniertes Image kann gehasht werden, besitzt aber keinen vertrauenswürdigen
  Authentizitätsnachweis.
