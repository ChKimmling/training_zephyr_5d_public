# Arbeitsblatt – Ereignispfad planen

## 1. Kontrollpfad: Wartungsanforderung

Ergänze die Reihenfolge vom Shell-Aufruf bis zur Annahme oder Ablehnung. Der Kontrollpfad
beschreibt, wie ein Bedienauftrag angenommen wird; er ist nicht die spätere Verarbeitung
der GPIO-Flanke.

```text
Servicetechniker
    -> [TODO: Shell-Kommando / Handler]
    -> [TODO: Validierung und Startanforderung]
    -> [TODO: GPIO-Stimulus]
    -> [TODO: unmittelbare Shell-Antwort]
```

**Arbeitskommentar:** <!-- TODO: Markiere, welche Schritte im Shell-Thread laufen und an welcher Stelle die Eingabe validiert sein muss. -->

## 2. Vollständiger Ablauf: ein gültiger Puls

Ordne die sechs Bausteine vom Shell-Aufruf bis zur abfragbaren Statistik. Der Ablauf enthält
zunächst den Kontrollpfad, dann die GPIO-Ereignisverarbeitung und schließlich die Abfrage.
Ein Worker darf den Shell- oder Callback-Kontext nicht blockieren.

```text
event pulse
    -> [TODO: Baustein 1]
    -> [TODO: Baustein 2]
    -> [TODO: Baustein 3]
    -> [TODO: Baustein 4]
    -> [TODO: Baustein 5]
    -> [TODO: Baustein 6]
    -> event stats / event status: Zustand abfragen
```

Die sechs Bausteine zur Auswahl:

- Shell-Handler
- Stimulusadapter
- GPIO-Emulator
- GPIO-Callback
- Event Service
- Queue-/Worker-Mechanismus

**Arbeitskommentar:** <!-- TODO: Ordne jedem Baustein genau eine Hauptverantwortung zu und notiere seinen Thread- oder Callback-Kontext. -->

## 3. Ausgaben und Ausführungskontext

| Situation | Shell-Antwort oder Log? | Ausführungskontext | Erwartung |
|---|---|---|---|
| gültiger Puls angenommen | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| GPIO-Flanke erkannt | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Event verarbeitet | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| ungültiger Parameter | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Stimulus bereits aktiv | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 4. Zählersemantik und Sollwerte

Definiere zuerst, was gezählt wird. Ein vollständiger Puls hat eine steigende und eine
fallende GPIO-Flanke; die Begriffe „Werkstück“, „Flanke“ und „verarbeiteter Eintrag“ sind
nicht automatisch gleichbedeutend.

| Szenario | Werkstücke angefordert | GPIO-Flanken (raw) | verarbeitet | verloren | abgewiesen |
|---|---:|---:|---:|---:|---:|
| `event pulse` erfolgreich, Worker abgearbeitet | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| `event burst 5 200` erfolgreich, Worker abgearbeitet | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| ungültiger Wert | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| neue Sequenz bei aktivem Stimulus | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

**Arbeitskommentar:** <!-- TODO: Dokumentiere, ob ein einzelnes Produkt als steigende Flanke oder als vollständiger Puls gilt. Berücksichtige asynchrone Verarbeitung und Queue-Überlauf. -->

## 5. Invariante

Formuliere eine mit `event stats` prüfbare Regel:

> <!-- TODO: Invariante für raw, processed, lost und/oder Werkstückzähler eintragen. -->
