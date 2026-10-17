# Changelog

## 0.4.0 - 2026-10-17
- A tap on a clock turns every world clock of the screen from dials to digits and back, and it stays so through a
  restart. Home Assistant sees it as a switch of the screen ("World clock digits"), to turn at night for example, and
  the screen's settings page (under "World clock") and the plugin's settings in the app have it too.
  From Tessera 0.4.89 on, the plugin's settings in the app follow a tap on the screen at once.

## 0.3.3 - 2026-10-12
- A picture on the Plugins page, and marked stable: it has run every day on several screens.

## 0.3.2 - 2026-10-11
- Where "Tomorrow · +7 h" does not fit after a place's name or under its clock, it says "+7 h" instead of being cut with dots.

## 0.3.1 - 2026-10-11
- Calm clock faces on low tiles too, from 6 mm, as Tessera's own calm clock on a card one row high: a 3x1 tile on a 4-inch screen shows dials again. A tile set to Classic or Calm keeps that face wherever a dial fits.

## 0.3.0 - 2026-10-11
- One second hand by default, on the home clock (the clock of the screen's own time, or the first one when none shows it): the seconds are the same in every zone, and three hands in step looked hectic. **Second hand** chooses the home clock, every clock or none; a tile that had the second hand switched off keeps none.

## 0.2.1 - 2026-10-11
- Builds with Tessera 0.4.87 and newer: the plugin now names plugin API 0.4, which those screens ask for. Nothing else changes.

## 0.2.0 - 2026-10-10
- Calm clock faces, as Tessera's own: a filled disc with four strokes and eight dots that reads at any size.
- Face chosen by size by default (calm on small dials, classic on large ones), or always Classic, Calm or Digits.

## 0.1.2 - 2026-10-09
- Clock faces on tiles one row high too: the offset then follows the place's name (New York  -6 h).

## 0.1.1 - 2026-10-09
- The offset says -6 h with a plain minus: the screen's fonts have no other.

## 0.1.0 - 2026-10-09
- First release: the time in up to three places of the world, as clock faces or in digits, with each place's offset
  from the screen's time. 46 cities, daylight saving included.
- Needs a Tessera with plugin API 0.7: update the app first.
