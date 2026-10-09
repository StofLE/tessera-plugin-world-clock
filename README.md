# World clock

The time in up to three places of the world on one tile: as clock faces drawn like Tessera's own analog clock, or in
digits. Each place has its name above its clock and how far it is from the screen's time below it ("-6 h",
"Tomorrow · +8 h"). The screen works the time out itself, daylight saving included: no entity, no internet service.

The places stand side by side. Each clock takes the largest size its column leaves; the line under it stays only where
it costs the clock nothing, else the offset follows the place's name ("New York  -6 h"). A column too narrow for a dial shows the time in digits, in the screen's own 24-hour or
AM/PM format.

## Set up

1. In Tessera, open **Plugins**, choose **World clock** and tick your screens. Each screen builds once.
2. In Layout, place the **World clock** tile from the library's Plugins group. Two or more columns wide gives each
   place room for its dial.
3. In the inspector, choose the three places. **Here** is the screen's own time zone, **None** leaves a place out.
   Give a place a name of your own if you like, such as the city you live in for **Here**.
4. Choose **Clock faces** or **Digits**, and whether the dials have a second hand.

## Good to know

- 46 places to choose from, every continent's large cities. A place follows its own daylight saving rules, north and
  south of the equator.
- The second hand moves once a second while the tile is on the glass.
- The screen needs its time from Home Assistant; until then the tile says it is waiting for it.

## How it works

- `tessera-plugin.yaml`: the tile and its options: three places, their names, the face and the second hand.
- `components/world_clock/`: the ESPHome component that draws the clocks and works out each place's time.
- `components/world_clock/zones.h`: each place's offset and daylight saving rules, written by `tools/make_zones.py`
  from the tzdata rules. Run it again when a country changes its rules.
- `translations/`: the words on the screen (`screen`) and in the app (`app`), the cities' names among them.
- `CHANGELOG.md`: what each version changed; the app shows it with an update.
