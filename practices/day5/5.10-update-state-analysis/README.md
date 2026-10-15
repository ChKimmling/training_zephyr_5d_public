# 5.10 – Update, Bestätigung und Revert analysieren

## Lernziel

Aus Imageheadern, Signaturinformationen, Slotbelegung und MCUboot-Trailerfeldern die
nächste Bootaktion ableiten. Bestätigten Stand, ausstehenden Testboot, unbestätigten
Teststand, erfolgreichen Abschluss und abzuweisenden Rollback-Kandidaten unterscheiden.

## Einordnung

`reference/` ist ein vollständiges Analyseprojekt ohne Teilnehmer-TODOs. Ein
`native_sim`-Build liefert die gemeinsame Ausgangsnutzlast. Die Hilfsskripte erzeugen
daraus getrennte, signierte Trainingsimages und vier reproduzierbare Slot-Szenarien.

Die Übung besitzt bewusst kein `start`-/`solution`-Paar: Untersucht werden vorbereitete
Zustandsbilder und MCUboot-Regeln, nicht zu ergänzender Anwendungscode. Die Szenarien
modellieren das Imageformat und ausgewählte Trailerfelder. Sie starten keinen echten
MCUboot und bilden keine unterbrochene sektorweise Swap-Operation nach.

## Build und Start

Die Befehle werden aus dem Workspace-Root ausgeführt; die gemeinsamen Voraussetzungen
stehen in der [Tagesübersicht](../README.md#voraussetzungen-und-befehle).

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day5/5.10-update-state-analysis/reference \
  -d build/day5-5.10
./build/day5-5.10/zephyr/zephyr.exe
```

`zephyr.exe` bleibt der ausführbare Hostprozess. `zephyr.bin` dient anschließend als
Basis für synthetische, nicht ausführbare MCUboot-Container dieser Analyseübung.

## Aufgabe

### 1. Szenarien vorbereiten

```sh
training-zephyr/practices/day5/5.10-update-state-analysis/scripts/prepare-scenarios.sh \
  build/day5-5.10 \
  build/day5-5.10/analysis
```

Der private Trainingsschlüssel entsteht ausschließlich unter `build/` und erhält Modus
`0600`. Das Skript leitet drei unterscheidbare Nutzlasten ab, signiert sie und erstellt:

- `01-pending-test`: bestätigter Stand im Primary, neuer Kandidat im Secondary,
- `02-test-unconfirmed`: neuer Stand im Primary, aber noch nicht bestätigt,
- `03-test-confirmed`: neuer Stand im Primary und bestätigt,
- `04-rollback-candidate`: gültig signierter, aber zu alter Kandidat im Secondary.

### 2. Alle Zustandsbilder kompakt lesen

```sh
for scenario_dir in build/day5-5.10/analysis/scenarios/*
do
  training-zephyr/practices/day5/5.10-update-state-analysis/scripts/inspect-scenario.py \
    "$scenario_dir" \
    build/day5-5.10/analysis/keys/training-ec-p256-public.pem
done
```

Das Werkzeug zeigt für jeden Slot Version, Security Counter, Signaturstatus, Trailer-
Magic, `copy_done` und `image_ok`. Es trifft bewusst keine Bootentscheidung.

### 3. Ausgangs- und Update-Image vergleichen

Untersuche Szenario `01-pending-test` zusätzlich direkt:

```sh
training-zephyr/practices/day5/5.10-update-state-analysis/scripts/imgtool.sh dumpinfo \
  build/day5-5.10/analysis/scenarios/01-pending-test/primary.bin
training-zephyr/practices/day5/5.10-update-state-analysis/scripts/imgtool.sh dumpinfo \
  build/day5-5.10/analysis/scenarios/01-pending-test/secondary.bin
```

Vergleiche Imageversion, Imagegröße, SHA-256-TLV, Key-Hash, ECDSA-Signatur und
`SEC_CNT`. Ordne anschließend den bestätigten Stand und den Updatekandidaten den beiden
Slots zu.

### 4. Bootentscheidung vor dem Testboot ableiten

Bewerte in `01-pending-test` die Kombination aus gültigem Secondary-Image, gültiger
Trailer-Magic und nicht gesetztem `image_ok`. Begründe, ob Normalstart, Testupdate,
permanentes Update oder Revert als nächste Aktion folgt.

Die vereinfachte Zuordnung dient als Einstieg:

```text
Secondary-Kandidat + image_ok nicht gesetzt  -> TEST
Secondary-Kandidat + image_ok gesetzt        -> PERM
Primary copy_done + image_ok nicht gesetzt   -> REVERT
```

Die reale MCUboot-Entscheidung wertet beide Trailer in definierter Priorität aus und
hängt zusätzlich vom konfigurierten Upgradeverfahren ab.

### 5. Bestätigung und Revert gegenüberstellen

Vergleiche `02-test-unconfirmed` mit `03-test-confirmed`:

- Welches Image liegt jeweils im Primary Slot?
- Welches Trailerfeld belegt den abgeschlossenen Kopiervorgang?
- Welches Trailerfeld wird erst nach erfolgreicher Selbstprüfung gesetzt?
- Welche Aktion folgt beim nächsten Reset, wenn diese Bestätigung fehlt?

Ordne `boot_write_img_confirmed()` fachlich ein. Die Bestätigung gehört hinter die für
das Produkt relevanten Selbstprüfungen und nicht unmittelbar an den Programmanfang.

### 6. Versions- und Security-Counter-Regel anwenden

Szenario `04-rollback-candidate` enthält ein kryptografisch gültiges Secondary-Image.
Die Szenariometadaten definieren jedoch den kleinsten zulässigen Security Counter:

```sh
cat build/day5-5.10/analysis/scenarios/04-rollback-candidate/scenario.json
```

Vergleiche Version und `SEC_CNT` beider Images mit der Regel. Begründe, warum eine
gültige Signatur allein keine Freigabe für ein Downgrade darstellt. Trenne dabei die
fachliche Versionsanzeige von der monotonen Anti-Rollback-Entscheidung.

### 7. Zustandsregeln im MCUboot-Code lokalisieren

```sh
sed -n '90,175p' \
  bootloader/mcuboot/boot/bootutil/src/bootutil_public.c
grep -n -E 'boot_(magic|image_ok|copy_done)_off' \
  bootloader/mcuboot/boot/bootutil/src/bootutil_misc.h
```

Suche die Regeln für `TEST`, `PERM` und `REVERT`. Vergleiche sie mit der vereinfachten
Tabelle aus Schritt 4. Achte darauf, dass die Tabellenreihenfolge Priorität ausdrückt
und dass Statuswerte beider Slots einbezogen werden.

### 8. Updateablauf dokumentieren

Erstelle zwei kurze Ablaufspuren und notiere an jedem Übergang das entscheidende
Artefakt:

```text
Kandidat im Secondary -> TEST -> Selbstprüfung -> image_ok -> dauerhaft
Kandidat im Secondary -> TEST -> Reset ohne image_ok -> REVERT -> alter Stand
```

Ergänze, wann Signatur, Version, Security Counter, Trailer-Magic, `copy_done` und
`image_ok` ausgewertet oder verändert werden.

## Hinweise zur Interpretation

- Slotinhalt und Trailerzustand müssen immer für denselben Beobachtungszeitpunkt gelesen
  werden.
- `copy_done`, `image_ok` und Trailer-Magic sind ein gemeinsamer Zustand, keine
  unabhängigen Statuslampen.
- Ein vollständiger Transfer in den Secondary Slot und ein unterbrochener MCUboot-Swap
  sind verschiedene Fehlerfälle.
- Persistente Daten müssen nach einem Revert weiterhin vom alten Image verstanden
  werden können.
- Die bereitgestellten Bilder modellieren Swap-basierte Grundzustände. Overwrite-only,
  Direct-XIP, RAM Load und Multi-Image-Updates besitzen abweichende Zustandsdetails.
- Produktionsentscheidungen benötigen die konkrete MCUboot-Konfiguration und dürfen
  nicht allein aus diesem Trainingsmodell abgeleitet werden.
