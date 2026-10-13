# 3.08 – Ereignispfad für den Use-Case planen

## Ziel

Vor der Implementierung in 3.10 und 3.12 Kontroll- und Ereignispfad trennen,
Modulverantwortungen zuordnen und prüfbare Sollwerte festlegen.

## Use-Case

Eine Lichtschranke am Förderband meldet vorbeifahrende Werkstücke. Im Wartungsmodus kann
ein Servicetechniker Ereignisse mit `event pulse` oder `event burst 5 200` simulieren.
Die Anwendung soll Eingaben validieren, GPIO-Events erfassen und sie außerhalb des
GPIO-Callback-Kontexts verarbeiten.

## Aufgabe

Bearbeite zuerst [start/sequence.md](start/sequence.md) allein oder in der Gruppe. Nutze
anschließend [solution/sequence.md](solution/sequence.md) zum Abgleich. Beide Blätter
verwenden dieselben Fragen, Tabellen und Diagrammplätze.

1. Trage die sechs Bausteine in den Kontroll- beziehungsweise Ereignispfad ein.
2. Ordne jedem Baustein Verantwortung und Ausführungskontext zu.
3. Lege fest, welche Ausgaben Shell-Antworten und welche interne Logs sind.
4. Unterscheide Werkstückzahl, GPIO-Flanken und Statistikzähler.
5. Bestimme Sollwerte für einen Puls, einen Fünfer-Burst und ungültige oder nicht zulässige
   Anforderungen.
6. Formuliere eine Invariante, die in 3.10 und 3.12 überprüft werden kann.

## Durchführung und Übergabe

Dies ist eine Architekturübung ohne eigenes Build. Halte die ausgefüllte Sequenzskizze
während 3.10 und 3.12 bereit. Änderungen an den hier festgelegten Zähler- oder Fehlerregeln
müssen begründet und in den jeweiligen README-Dateien dokumentiert werden.

## Abnahmekriterien

- Kontrollpfad und Ereignispfad sind getrennt dargestellt.
- Alle Bausteine besitzen eine klare Verantwortung und einen Ausführungskontext.
- Ein vollständiger Puls wird nicht mit einer einzelnen GPIO-Flanke verwechselt.
- Ungültige Eingaben erzeugen keinen GPIO-Stimulus und erhöhen keine Werkstückzahl.
- Sollwerte erlauben die spätere Überprüfung auch bei asynchroner Verarbeitung.
