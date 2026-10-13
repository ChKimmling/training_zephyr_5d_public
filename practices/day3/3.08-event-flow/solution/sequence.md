# Referenz – Ereignispfad planen

## 1. Kontrollpfad: Wartungsanforderung

```text
Servicetechniker
    -> Shell-Kommando / Handler
    -> Argumente vollständig validieren
    -> Stimulusadapter um Puls/Burst bitten
    -> Shell: Auftrag angenommen ODER verständlicher Fehler
```

Der gesamte Kontrollpfad läuft im Shell-Thread. Zahlen, Wertebereiche und Busy-Zustand
werden geprüft, bevor ein Stimulus gestartet wird. Die Antwort „angenommen“ bestätigt nur,
dass die Sequenz gestartet wurde, nicht dass ihre GPIO-Flanken bereits verarbeitet sind.

## 2. Ereignispfad: ein gültiger Puls

```text
Shell-Handler
    -> Stimulusadapter
    -> GPIO-Emulator
    -> GPIO-Callback
    -> Event Service: rohe Flanke erfassen und Eintrag non-blocking einreihen
    -> Queue-Worker: Eintrag außerhalb des Callbacks verarbeiten
    -> Event Service: Statistik aktualisieren
    -> event stats / event status: konsistenten Snapshot abfragen
```

Verantwortungen und Kontexte:

| Baustein | Hauptverantwortung | Kontext |
|---|---|---|
| Shell-Handler | Syntax und Wertebereiche prüfen, öffentliches API aufrufen, Bedienergebnis melden | Shell-Thread |
| Stimulusadapter | Emulierten Eingangspegel zeitgesteuert ändern; Puls/Burst nicht blockierend ausführen | Delayable-Work/System-Workqueue |
| GPIO-Emulator | Hardwareunabhängig Pegeländerungen als GPIO-Eingang abbilden | Treiber/API |
| GPIO-Callback | Flanke erkennen und minimale Service-Funktion aufrufen; nicht blockieren und keine Shell-Ausgabe | GPIO-Callback-Kontext |
| Event Service | Zähler, Fehler und Servicezustand verwalten; rohe Events in begrenzte Queue stellen | API im Aufruferkontext plus synchronisierter Zustand |
| Queue-Worker | Eingereihte Events asynchron verarbeiten und Verarbeitungszähler erhöhen | Worker-Thread |

Der Worker kann als eigener Service-Thread mit Message-Queue umgesetzt werden, wie in
3.12, oder in einer anderen Übung als Workqueue-Arbeit. Er ist ein Ausführungsmechanismus,
keine zusätzliche Shell- oder GPIO-Schicht.

## 3. Ausgaben und Ausführungskontext

| Situation | Shell-Antwort oder Log? | Ausführungskontext | Erwartung |
|---|---|---|---|
| gültiger Puls angenommen | Shell-Antwort | Shell-Thread | `pulse accepted`; Verarbeitung läuft danach asynchron |
| GPIO-Flanke erkannt | keine Bedienausgabe; optional internes Debug-Log | GPIO-Callback | kurze Erfassung und Einreihung, kein Blockieren |
| Event verarbeitet | internes Log nur bei Diagnosebedarf; Ergebnis über Statistik abfragen | Worker-Thread | processed-Zähler nach erfolgreicher Verarbeitung erhöhen |
| ungültiger Parameter | Shell-Fehler | Shell-Thread | verständliche Meldung; kein Stimulus und keine Flanke |
| Stimulus bereits aktiv | Shell-Fehler | Shell-Thread | Busy-Fehler; kein weiterer Stimulus |

Logging ist für interne Diagnose, Shell-Ausgaben für unmittelbare Bedienergebnisse und
Abfragen. Der Callback schreibt nicht auf die Shell.

## 4. Zählersemantik und Sollwerte

Ein vollständiger Puls wechselt den Pegel von inaktiv zu aktiv und zurück. Der GPIO-Callback
ist auf beide Flanken konfiguriert; `raw_count` zählt daher Flanken, nicht Werkstücke.
Ein Puls erzeugt zwei rohe Events. Ein Fünfer-Burst erzeugt zehn. Die Event-Queue verarbeitet
jede Flanke separat. Nach vollständiger Abarbeitung ohne Queue-Überlauf gilt `processed=raw`
und `lost=0`.

| Szenario | Werkstücke angefordert | GPIO-Flanken (raw) | verarbeitet | verloren | abgewiesen |
|---|---:|---:|---:|---:|---:|
| `event pulse` erfolgreich, Worker abgearbeitet | 1 | 2 | 2 | 0 | 0 |
| `event burst 5 200` erfolgreich, Worker abgearbeitet | 5 | 10 | 10 | 0 | 0 |
| ungültiger Wert | 0 | 0 | 0 | 0 | 1 |
| neue Sequenz bei aktivem Stimulus | 0 zusätzliche | 0 zusätzliche | 0 zusätzliche | 0 zusätzliche | 1 |

Die Pulse sind nicht die fachliche Werkstückzählung: Wollte eine Anwendung Produkte statt
Flanken zählen, müsste sie beispielsweise nur steigende Flanken als Werkstückereignis werten
oder einen separaten Produktzähler führen. Für die Übungen 3.10/3.12 sind die genannten
`raw`- und `processed`-Zähler ausdrücklich Flankenzähler. Der Abstand im Burst beginnt gemäß
dem Vertrag von 3.10 nach dem Ende eines Pulses.

Die Tabellenwerte für `processed` gelten erst, nachdem der asynchrone Worker die Queue
abgearbeitet hat. Während des Ablaufs darf `processed` hinter `raw` liegen. Wenn die Queue
voll ist, wird ein Event als `lost` gezählt und nicht als verarbeitet.

## 5. Invariante

> Nach dem Leeren der Queue gilt `raw = processed + lost`. Jeder GPIO-Flanken-Eintrag wird
> höchstens einmal verarbeitet; abgewiesene Bedienanforderungen verändern weder `raw` noch
> `processed` noch `lost`.

Zusätzlich erzeugt jede vollständig beendete Pulssequenz genau zwei Flanken pro Puls.
