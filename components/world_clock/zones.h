#pragma once
// Written by tools/make_zones.py from the tzdata POSIX rules: do not edit.
#include <cstdint>

namespace esphome::world_clock {

// When daylight saving starts or ends: the `week`th `weekday` (0 Sunday) of `month`, `time` seconds after
// midnight local time (week 5 is the last). A month of 0 is no rule.
struct Rule { uint8_t month, week, weekday; int32_t time; };
// A zone: its offset east of UTC in standard and in daylight saving time, and when that starts and ends.
struct Zone { const char *id; int32_t standard, daylight; Rule start, end; };

inline constexpr Zone ZONES[] = {
    {"Pacific/Honolulu", -36000, -36000, {0, 0, 0, 0}, {0, 0, 0, 0}},  // HST10
    {"America/Anchorage", -32400, -28800, {3, 2, 0, 7200}, {11, 1, 0, 7200}},  // AKST9AKDT,M3.2.0,M11.1.0
    {"America/Los_Angeles", -28800, -25200, {3, 2, 0, 7200}, {11, 1, 0, 7200}},  // PST8PDT,M3.2.0,M11.1.0
    {"America/Denver", -25200, -21600, {3, 2, 0, 7200}, {11, 1, 0, 7200}},  // MST7MDT,M3.2.0,M11.1.0
    {"America/Chicago", -21600, -18000, {3, 2, 0, 7200}, {11, 1, 0, 7200}},  // CST6CDT,M3.2.0,M11.1.0
    {"America/Mexico_City", -21600, -21600, {0, 0, 0, 0}, {0, 0, 0, 0}},  // CST6
    {"America/New_York", -18000, -14400, {3, 2, 0, 7200}, {11, 1, 0, 7200}},  // EST5EDT,M3.2.0,M11.1.0
    {"America/Toronto", -18000, -14400, {3, 2, 0, 7200}, {11, 1, 0, 7200}},  // EST5EDT,M3.2.0,M11.1.0
    {"America/Bogota", -18000, -18000, {0, 0, 0, 0}, {0, 0, 0, 0}},  // <-05>5
    {"America/Lima", -18000, -18000, {0, 0, 0, 0}, {0, 0, 0, 0}},  // <-05>5
    {"America/Santiago", -14400, -10800, {9, 1, 6, 86400}, {4, 1, 6, 86400}},  // <-04>4<-03>,M9.1.6/24,M4.1.6/24
    {"America/Sao_Paulo", -10800, -10800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // <-03>3
    {"America/Argentina/Buenos_Aires", -10800, -10800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // <-03>3
    {"Atlantic/Reykjavik", 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}},  // GMT0
    {"Europe/London", 0, 3600, {3, 5, 0, 3600}, {10, 5, 0, 7200}},  // GMT0BST,M3.5.0/1,M10.5.0
    {"Europe/Dublin", 3600, 0, {10, 5, 0, 7200}, {3, 5, 0, 3600}},  // IST-1GMT0,M10.5.0,M3.5.0/1
    {"Europe/Lisbon", 0, 3600, {3, 5, 0, 3600}, {10, 5, 0, 7200}},  // WET0WEST,M3.5.0/1,M10.5.0
    {"Europe/Madrid", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Paris", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Amsterdam", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Berlin", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Zurich", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Rome", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Vienna", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Stockholm", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Warsaw", 3600, 7200, {3, 5, 0, 7200}, {10, 5, 0, 10800}},  // CET-1CEST,M3.5.0,M10.5.0/3
    {"Europe/Athens", 7200, 10800, {3, 5, 0, 10800}, {10, 5, 0, 14400}},  // EET-2EEST,M3.5.0/3,M10.5.0/4
    {"Europe/Helsinki", 7200, 10800, {3, 5, 0, 10800}, {10, 5, 0, 14400}},  // EET-2EEST,M3.5.0/3,M10.5.0/4
    {"Europe/Istanbul", 10800, 10800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // <+03>-3
    {"Europe/Moscow", 10800, 10800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // MSK-3
    {"Africa/Cairo", 7200, 10800, {4, 5, 5, 0}, {10, 5, 4, 86400}},  // EET-2EEST,M4.5.5/0,M10.5.4/24
    {"Africa/Lagos", 3600, 3600, {0, 0, 0, 0}, {0, 0, 0, 0}},  // WAT-1
    {"Africa/Johannesburg", 7200, 7200, {0, 0, 0, 0}, {0, 0, 0, 0}},  // SAST-2
    {"Africa/Nairobi", 10800, 10800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // EAT-3
    {"Asia/Dubai", 14400, 14400, {0, 0, 0, 0}, {0, 0, 0, 0}},  // <+04>-4
    {"Asia/Karachi", 18000, 18000, {0, 0, 0, 0}, {0, 0, 0, 0}},  // PKT-5
    {"Asia/Kolkata", 19800, 19800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // IST-5:30
    {"Asia/Bangkok", 25200, 25200, {0, 0, 0, 0}, {0, 0, 0, 0}},  // <+07>-7
    {"Asia/Singapore", 28800, 28800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // <+08>-8
    {"Asia/Shanghai", 28800, 28800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // CST-8
    {"Asia/Hong_Kong", 28800, 28800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // HKT-8
    {"Asia/Seoul", 32400, 32400, {0, 0, 0, 0}, {0, 0, 0, 0}},  // KST-9
    {"Asia/Tokyo", 32400, 32400, {0, 0, 0, 0}, {0, 0, 0, 0}},  // JST-9
    {"Australia/Perth", 28800, 28800, {0, 0, 0, 0}, {0, 0, 0, 0}},  // AWST-8
    {"Australia/Sydney", 36000, 39600, {10, 1, 0, 7200}, {4, 1, 0, 10800}},  // AEST-10AEDT,M10.1.0,M4.1.0/3
    {"Pacific/Auckland", 43200, 46800, {9, 5, 0, 7200}, {4, 1, 0, 10800}},  // NZST-12NZDT,M9.5.0,M4.1.0/3
};

}  // namespace esphome::world_clock
