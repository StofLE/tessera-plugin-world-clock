#!/usr/bin/env python3
"""Write the world clock's zones: components/world_clock/zones.h (each zone's offset from UTC and its daylight saving
rules, read from the POSIX rule at the end of the system's tzdata file) and the cities' names in translations/.

    python3 tools/make_zones.py              # reads /usr/share/zoneinfo
    python3 tools/make_zones.py /path/to/zoneinfo

A tile chooses a zone from this list; the screen works the local time out itself, daylight saving included. The list
is at most 48 long with "local" (a choice's limit in the manifest). Run it again when tzdata changes a rule.
"""
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent.parent

# (IANA id, English name, German name). "local" is the screen's own zone.
ZONES = [
    ('local', 'Here', 'Hier'),
    ('Pacific/Honolulu', 'Honolulu', 'Honolulu'),
    ('America/Anchorage', 'Anchorage', 'Anchorage'),
    ('America/Los_Angeles', 'Los Angeles', 'Los Angeles'),
    ('America/Denver', 'Denver', 'Denver'),
    ('America/Chicago', 'Chicago', 'Chicago'),
    ('America/Mexico_City', 'Mexico City', 'Mexiko-Stadt'),
    ('America/New_York', 'New York', 'New York'),
    ('America/Toronto', 'Toronto', 'Toronto'),
    ('America/Bogota', 'Bogotá', 'Bogotá'),
    ('America/Lima', 'Lima', 'Lima'),
    ('America/Santiago', 'Santiago', 'Santiago'),
    ('America/Sao_Paulo', 'São Paulo', 'São Paulo'),
    ('America/Argentina/Buenos_Aires', 'Buenos Aires', 'Buenos Aires'),
    ('Atlantic/Reykjavik', 'Reykjavík', 'Reykjavík'),
    ('Europe/London', 'London', 'London'),
    ('Europe/Dublin', 'Dublin', 'Dublin'),
    ('Europe/Lisbon', 'Lisbon', 'Lissabon'),
    ('Europe/Madrid', 'Madrid', 'Madrid'),
    ('Europe/Paris', 'Paris', 'Paris'),
    ('Europe/Amsterdam', 'Amsterdam', 'Amsterdam'),
    ('Europe/Berlin', 'Berlin', 'Berlin'),
    ('Europe/Zurich', 'Zurich', 'Zürich'),
    ('Europe/Rome', 'Rome', 'Rom'),
    ('Europe/Vienna', 'Vienna', 'Wien'),
    ('Europe/Stockholm', 'Stockholm', 'Stockholm'),
    ('Europe/Warsaw', 'Warsaw', 'Warschau'),
    ('Europe/Athens', 'Athens', 'Athen'),
    ('Europe/Helsinki', 'Helsinki', 'Helsinki'),
    ('Europe/Istanbul', 'Istanbul', 'Istanbul'),
    ('Europe/Moscow', 'Moscow', 'Moskau'),
    ('Africa/Cairo', 'Cairo', 'Kairo'),
    ('Africa/Lagos', 'Lagos', 'Lagos'),
    ('Africa/Johannesburg', 'Johannesburg', 'Johannesburg'),
    ('Africa/Nairobi', 'Nairobi', 'Nairobi'),
    ('Asia/Dubai', 'Dubai', 'Dubai'),
    ('Asia/Karachi', 'Karachi', 'Karatschi'),
    ('Asia/Kolkata', 'Mumbai', 'Mumbai'),
    ('Asia/Bangkok', 'Bangkok', 'Bangkok'),
    ('Asia/Singapore', 'Singapore', 'Singapur'),
    ('Asia/Shanghai', 'Shanghai', 'Shanghai'),
    ('Asia/Hong_Kong', 'Hong Kong', 'Hongkong'),
    ('Asia/Seoul', 'Seoul', 'Seoul'),
    ('Asia/Tokyo', 'Tokyo', 'Tokio'),
    ('Australia/Perth', 'Perth', 'Perth'),
    ('Australia/Sydney', 'Sydney', 'Sydney'),
    ('Pacific/Auckland', 'Auckland', 'Auckland'),
]

NAME = r'(?:[A-Za-z]{3,}|<[^>]+>)'
OFFSET = r'([+-]?\d{1,2}(?::\d{2}){0,2})'
RULE = r'M(\d{1,2})\.(\d)\.(\d)(?:/([+-]?\d{1,3}(?::\d{2}){0,2}))?'


def seconds(text, default=0):
    """'5', '-5:30', '+2' as seconds (hours, minutes, seconds); `default` for none."""
    if text is None:
        return default
    sign = -1 if text.startswith('-') else 1
    parts = [int(p) for p in text.lstrip('+-').split(':')] + [0, 0]
    return sign * (parts[0] * 3600 + parts[1] * 60 + parts[2])


def parse(posix):
    """(std, dst, start, end) of a POSIX TZ rule, offsets east of UTC in seconds, rules (month, week, weekday, time)."""
    m = re.fullmatch(rf'{NAME}{OFFSET}(?:{NAME}{OFFSET}?,{RULE},{RULE})?', posix)
    if not m:
        raise SystemExit(f'cannot read the rule "{posix}"')
    std = -seconds(m.group(1))
    if not m.group(3):
        return std, std, None, None
    dst = -seconds(m.group(2), -std - 3600) if m.group(2) else std + 3600
    start = (int(m.group(3)), int(m.group(4)), int(m.group(5)), seconds(m.group(6), 7200))
    end = (int(m.group(7)), int(m.group(8)), int(m.group(9)), seconds(m.group(10), 7200))
    return std, dst, start, end


def main():
    zoneinfo = Path(sys.argv[1] if len(sys.argv) > 1 else '/usr/share/zoneinfo')
    assert len(ZONES) <= 47, 'a choice takes at most 48, None among them'
    rows = []
    for zone, _, _ in ZONES:
        if zone == 'local':
            continue
        posix = (zoneinfo / zone).read_bytes().rstrip(b'\n').split(b'\n')[-1].decode()
        std, dst, start, end = parse(posix)
        rule = lambda r: '{%d, %d, %d, %d}' % r if r else '{0, 0, 0, 0}'
        rows.append(f'    {{"{zone}", {std}, {dst}, {rule(start)}, {rule(end)}}},  // {posix}')
    lines = [
        '#pragma once',
        '// Written by tools/make_zones.py from the tzdata POSIX rules: do not edit.',
        '#include <cstdint>',
        '',
        'namespace esphome::world_clock {',
        '',
        '// When daylight saving starts or ends: the `week`th `weekday` (0 Sunday) of `month`, `time` seconds after',
        '// midnight local time (week 5 is the last). A month of 0 is no rule.',
        'struct Rule { uint8_t month, week, weekday; int32_t time; };',
        '// A zone: its offset east of UTC in standard and in daylight saving time, and when that starts and ends.',
        'struct Zone { const char *id; int32_t standard, daylight; Rule start, end; };',
        '',
        'inline constexpr Zone ZONES[] = {',
        *rows,
        '};',
        '',
        '}  // namespace esphome::world_clock',
        '',
    ]
    (HERE / 'components' / 'world_clock' / 'zones.h').write_text('\n'.join(lines))
    # The cities' names: in the app (the editor's list) and on the screen (the name over a clock).
    for language, column in (('en', 1), ('de', 2)):
        path = HERE / 'translations' / f'{language}.json'
        data = json.loads(path.read_text())
        for zone in ZONES:
            key = 'z_' + re.sub(r'[^a-z0-9]+', '_', zone[0].lower()).strip('_')
            data['app'][key] = zone[column]
            data['screen'][key] = zone[column]
        path.write_text(json.dumps(data, ensure_ascii=False, indent=2) + '\n')
    print(f'zones.h: {len(rows)} zones and local')


if __name__ == '__main__':
    main()
