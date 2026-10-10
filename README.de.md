# Weltzeituhr

Die Uhrzeit an bis zu drei Orten der Welt auf einem Tile: als Zifferblätter, gezeichnet wie Tesseras eigene
Analoguhr (klassisch mit Strichen und Ziffern, oder ruhig), oder in Ziffern. Über jeder Uhr steht der Name des Orts, darunter, wie weit er von der Zeit des Bildschirms
entfernt ist („-6 h“, „Morgen · +8 h“). Der Bildschirm rechnet die Zeit selbst aus, Sommerzeit eingeschlossen: keine
Entität, kein Internetdienst.

Die Orte stehen nebeneinander. Jede Uhr bekommt die größte Größe, die ihre Spalte lässt; die Zeile darunter bleibt nur,
wo sie die Uhr nichts kostet, sonst steht die Abweichung hinter dem Namen des Orts („New York  -6 h“). Eine Spalte, die für ein Zifferblatt zu schmal ist, zeigt die Zeit in Ziffern, im
24-Stunden- oder AM/PM-Format des Bildschirms.

## Einrichten

1. Öffne in Tessera **Plugins**, wähle **Weltzeituhr** und hake deine Bildschirme an. Jeder Bildschirm baut einmal neu.
2. Platziere im Layout das Tile **Weltzeituhr** aus der Gruppe Plugins der Bibliothek. Mit zwei oder mehr Spalten
   Breite hat jeder Ort Platz für sein Zifferblatt.
3. Wähle im Inspector die drei Orte. **Hier** ist die Zeitzone des Bildschirms, **Keiner** lässt einen Ort weg. Gib
   einem Ort bei Bedarf einen eigenen Namen, etwa deine Stadt für **Hier**.
4. Wähle die Darstellung: **Zifferblätter, nach Größe** (ruhige bei kleinen Uhren, klassische bei großen), **Klassisch**,
   **Ruhig** oder **Ziffern**, und ob die Zifferblätter einen Sekundenzeiger haben.

## Gut zu wissen

- 46 Orte zur Auswahl, die großen Städte aller Kontinente. Jeder Ort folgt seinen eigenen Sommerzeit-Regeln, nördlich
  und südlich des Äquators.
- Der Sekundenzeiger bewegt sich einmal pro Sekunde, solange das Tile auf dem Glas ist.
- Der Bildschirm braucht seine Uhrzeit von Home Assistant; bis dahin sagt das Tile, dass es darauf wartet.

## So funktioniert es

- `tessera-plugin.yaml`: das Tile und seine Optionen: drei Orte, ihre Namen, die Darstellung und der Sekundenzeiger.
- `components/world_clock/`: die ESPHome-Komponente, die die Uhren zeichnet und die Zeit jedes Orts ausrechnet.
- `components/world_clock/zones.h`: Abweichung und Sommerzeit-Regeln jedes Orts, geschrieben von
  `tools/make_zones.py` aus den tzdata-Regeln. Neu ausführen, wenn ein Land seine Regeln ändert.
- `translations/`: die Worte auf dem Bildschirm (`screen`) und in der App (`app`), darunter die Namen der Städte.
- `CHANGELOG.md`: was jede Version geändert hat; die App zeigt es bei einem Update.
