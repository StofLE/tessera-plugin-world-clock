#include "world_clock.h"
#include "zones.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

namespace esphome::world_clock {

using tessera::Font;
namespace ui = tessera::ui;

static constexpr int MAX_CLOCKS = 3;
static constexpr float PI = 3.14159265f;

// ---- Time in a zone ----

// Days since 1970-01-01 of a date, and back (Howard Hinnant's algorithms).
static int32_t days_from_civil(int y, int m, int d) {
  y -= m <= 2;
  const int era = (y >= 0 ? y : y - 399) / 400;
  const int yoe = y - era * 400;
  const int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  return era * 146097 + yoe * 365 + yoe / 4 - yoe / 100 + doy - 719468;
}
static void civil_from_days(int32_t z, int &y, int &m, int &d) {
  z += 719468;
  const int era = (z >= 0 ? z : z - 146096) / 146097;
  const int doe = z - era * 146097;
  const int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  const int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  const int mp = (5 * doy + 2) / 153;
  d = doy - (153 * mp + 2) / 5 + 1;
  m = mp < 10 ? mp + 3 : mp - 9;
  y = yoe + era * 400 + (m <= 2);
}
static int days_in_month(int y, int m) {
  return days_from_civil(m == 12 ? y + 1 : y, m == 12 ? 1 : m + 1, 1) - days_from_civil(y, m, 1);
}

// The moment (UTC) a daylight saving rule turns in `year`, with the offset that holds before it.
static int64_t transition(int year, const Rule &rule, int32_t before) {
  const int32_t first = days_from_civil(year, rule.month, 1);
  const int first_weekday = static_cast<int>(((first % 7) + 11) % 7);  // 0 Sunday; 1970-01-01 was a Thursday
  int day = 1 + (rule.weekday - first_weekday + 7) % 7 + 7 * (rule.week - 1);
  while (day > days_in_month(year, rule.month)) day -= 7;
  return static_cast<int64_t>(first + day - 1) * 86400 + rule.time - before;
}

// A zone's offset east of UTC at a moment, daylight saving included (north and south of the equator alike).
static int32_t offset_at(const Zone &zone, int64_t utc) {
  if (!zone.start.month) return zone.standard;
  int y, m, d;
  civil_from_days(static_cast<int32_t>((utc + zone.standard) / 86400), y, m, d);
  const int64_t start = transition(y, zone.start, zone.standard), end = transition(y, zone.end, zone.daylight);
  const bool daylight = start < end ? utc >= start && utc < end : !(utc >= end && utc < start);
  return daylight ? zone.daylight : zone.standard;
}

// The screen's own offset east of UTC at a moment, from its clock.
static int32_t local_offset(uint32_t utc) {
  const tessera::LocalTime t = tessera::local_time(utc);
  const int64_t wall = static_cast<int64_t>(days_from_civil(t.year, t.month, t.day)) * 86400 + t.hour * 3600 +
                       t.minute * 60 + t.second;
  return static_cast<int32_t>(wall - utc);
}

static const Zone *zone_named(const char *id) {
  for (const Zone &zone : ZONES)
    if (strcmp(zone.id, id) == 0) return &zone;
  return nullptr;
}

// "z_america_new_york" for "America/New_York": the key of a city's name in translations/.
static std::string name_key(const std::string &id) {
  std::string key = "z_";
  for (char c : id) key += (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ? c : '_';
  return key;
}

// ---- The tile ----
//
// Each place is a column: its name, its clock (a dial as Tessera's own analog clock draws it, or the time in digits)
// and how far it is from the screen's time. The clock takes the largest size the column leaves; the offset line goes
// where it would cost the clock, and the offset then follows the name. A column too small for a dial shows digits.

class ClocksTile : public tessera::Tile {
 public:
  explicit ClocksTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    width_ = c.width;
    height_ = c.height;
    analog_ = strcmp(c.options["face"] | "analog", "digital") != 0;
    seconds_ = c.options["seconds"] | true;
    static const char *const ZONE_OPTIONS[MAX_CLOCKS] = {"zone_1", "zone_2", "zone_3"};
    static const char *const NAME_OPTIONS[MAX_CLOCKS] = {"name_1", "name_2", "name_3"};
    for (int i = 0; i < MAX_CLOCKS; ++i) {
      const std::string id = c.options[ZONE_OPTIONS[i]] | "none";
      if (id == "none") continue;
      Place &p = places_[count_];
      p.local = id == "local";
      p.zone = p.local ? nullptr : zone_named(id.c_str());
      if (!p.local && !p.zone) continue;
      p.name = c.options[NAME_OPTIONS[i]] | "";
      if (p.name.empty()) p.name = plugin_->text(name_key(id).c_str());
      ++count_;
    }
    note_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    lv_obj_set_style_text_align(note_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(note_, width_, ui::line_height(Font::BODY));
    lv_obj_set_pos(note_, 0, (height_ - ui::line_height(Font::BODY)) / 2);
    lv_obj_add_flag(note_, LV_OBJ_FLAG_HIDDEN);
    if (!count_) return;
    lay_out(c.parent);
  }

  void on_tick(uint32_t epoch) override {
    if (!count_) return;
    if (!epoch) {
      for (int i = 0; i < count_; ++i) show(places_[i], false);
      ui::set_text(note_, plugin_->text("waiting"));
      lv_obj_remove_flag(note_, LV_OBJ_FLAG_HIDDEN);
      return;
    }
    lv_obj_add_flag(note_, LV_OBJ_FLAG_HIDDEN);
    const int32_t here = local_offset(epoch);
    const int32_t today = static_cast<int32_t>((static_cast<int64_t>(epoch) + here) / 86400);
    for (int i = 0; i < count_; ++i) {
      Place &p = places_[i];
      show(p, true);
      const int32_t offset = p.local ? here : offset_at(*p.zone, epoch);
      const int64_t wall = static_cast<int64_t>(epoch) + offset;
      const int64_t seconds = ((wall % 86400) + 86400) % 86400;
      if (analog_ && p.dial) {
        set_hand(p, p.hour, p.hour_points, (seconds % 43200) / 43200.0f, 0.52f, 0.f);
        set_hand(p, p.minute, p.minute_points, (seconds % 3600) / 3600.0f, 0.82f, 0.f);
        if (p.second) set_hand(p, p.second, p.second_points, (seconds % 60) / 60.0f, 0.92f, 0.2f);
      } else {
        // The time as the screen writes its own (24 hours or AM/PM): its clock text of the moment whose local time
        // is this place's.
        ui::set_text(p.time, tessera::clock_text(static_cast<uint32_t>(static_cast<int64_t>(epoch) + offset - here)));
      }
      const std::string away = offset_text(epoch, offset, here, static_cast<int32_t>(wall / 86400) - today);
      if (p.offset) ui::set_text(p.offset, away);
      // No line for it: the offset after the name, as the departures plugin writes a delay after where a bus goes.
      else ui::set_text(p.name_label, offset == here ? p.name : p.name + "  " + away);
    }
  }

  void on_theme() override {
    ui::set_color(note_, theme::MUTED);
    for (int i = 0; i < count_; ++i) {
      Place &p = places_[i];
      ui::set_color(p.name_label, theme::MUTED);
      if (p.time) ui::set_color(p.time, theme::INK);
      if (p.offset) ui::set_color(p.offset, theme::MUTED);
      for (lv_obj_t *mark : p.marks)
        if (mark) lv_obj_set_style_line_color(mark, ui::color(theme::MUTED), 0);
      for (lv_obj_t *numeral : p.numerals)
        if (numeral) ui::set_color(numeral, theme::MUTED);
      if (p.hour) lv_obj_set_style_line_color(p.hour, ui::color(theme::INK), 0);
      if (p.minute) lv_obj_set_style_line_color(p.minute, ui::color(theme::ACCENT), 0);
      if (p.second) lv_obj_set_style_line_color(p.second, theme::rgb(theme::foreground(theme::ha::ALARM)), 0);
      if (p.dot) lv_obj_set_style_bg_color(p.dot, ui::color(theme::ACCENT), 0);
    }
  }

 private:
  struct Place {
    bool local = false;
    const Zone *zone = nullptr;
    std::string name;
    lv_obj_t *name_label{}, *time{}, *offset{};
    // A dial: twelve marks (or numerals at 12, 3, 6 and 9), the hands and the centre dot.
    bool dial = false;
    int cx = 0, cy = 0, radius = 0;
    lv_obj_t *marks[12]{}, *numerals[4]{}, *hour{}, *minute{}, *second{}, *dot{};
    lv_point_precise_t mark_points[12][2]{}, hour_points[2]{}, minute_points[2]{}, second_points[2]{};
  };

  // "-6 h", "+5:30 h", "Tomorrow · +8 h"; for the screen's own place its date.
  std::string offset_text(uint32_t epoch, int32_t offset, int32_t here, int32_t days) const {
    const int32_t diff = offset - here;
    std::string text;
    if (diff == 0) {
      text = plugin_->text("same");
    } else {
      const int minutes = static_cast<int>(std::abs(diff) / 60);
      char n[16];
      if (minutes % 60) snprintf(n, sizeof n, "%s%d:%02d", diff < 0 ? "-" : "+", minutes / 60, minutes % 60);
      else snprintf(n, sizeof n, "%s%d", diff < 0 ? "-" : "+", minutes / 60);
      text = tessera::fill(plugin_->text("offset"), "n", n);
    }
    if (days > 0) return std::string(plugin_->text("tomorrow")) + " · " + text;
    if (days < 0) return std::string(plugin_->text("yesterday")) + " · " + text;
    return text;
  }

  static void show(Place &p, bool shown) {
    auto set = [shown](lv_obj_t *o) {
      if (!o) return;
      if (shown) lv_obj_remove_flag(o, LV_OBJ_FLAG_HIDDEN);
      else lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
    };
    set(p.name_label);
    set(p.time);
    set(p.offset);
    for (lv_obj_t *o : p.marks) set(o);
    for (lv_obj_t *o : p.numerals) set(o);
    set(p.hour);
    set(p.minute);
    set(p.second);
    set(p.dot);
  }

  lv_obj_t *line(lv_obj_t *parent, int width) {
    lv_obj_t *l = lv_line_create(parent);
    lv_obj_remove_flag(l, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_line_width(l, width, 0);
    lv_obj_set_style_line_rounded(l, true, 0);
    return l;
  }

  // A hand of the dial at `turn` (0 to 1 from 12 o'clock), `length` of the radius long, with a tail of `tail`.
  static void set_hand(const Place &p, lv_obj_t *hand, lv_point_precise_t *points, float turn, float length, float tail) {
    const float a = turn * 2 * PI, s = sinf(a), c = cosf(a);
    points[0] = {static_cast<lv_value_precise_t>(p.cx - p.radius * tail * s), static_cast<lv_value_precise_t>(p.cy + p.radius * tail * c)};
    points[1] = {static_cast<lv_value_precise_t>(p.cx + p.radius * length * s), static_cast<lv_value_precise_t>(p.cy - p.radius * length * c)};
    lv_line_set_points(hand, points, 2);
  }

  // The columns, made once: a tile's size and options never change while it lives.
  void lay_out(lv_obj_t *parent) {
    const bool large = ui::large();
    const int gap = ui::px(large ? 16 : 8);
    const int column = (width_ - (count_ - 1) * gap) / count_;
    const int name_h = ui::line_height(Font::BODY), line_gap = ui::px(large ? 6 : 3);
    // The offset line stays where the clock keeps its size without it, as the departures plugin keeps its later times.
    const int room_with = height_ - 2 * (name_h + line_gap), room_without = height_ - name_h - line_gap;
    // A dial where it has 10 mm without the offset line; the offset then follows the place's name instead.
    const bool dials = analog_ && std::min(column, room_without) >= ui::mm(10);
    Font face = Font::BODY;
    bool with_offset = true;
    if (dials) {
      with_offset = std::min(column, room_with) >= std::min(column, room_without);
    } else {
      // The largest digits for the shape of a time ("00:00", "00:00 AM") that fit a column.
      const std::string shape = digits_as_zero(tessera::clock_text(tessera::epoch() ? tessera::epoch() : 43200));
      auto fitting = [&](int room) {
        for (Font f : {Font::VALUE, Font::HEADLINE, Font::TITLE, Font::BODY_LARGE})
          if (ui::text_width(shape, f) <= column && ui::line_height(f) <= room) return f;
        return Font::BODY;
      };
      face = fitting(room_with);
      with_offset = face == fitting(room_without);
    }
    const int clock_room = with_offset ? room_with : room_without;
    const int dial = dials ? std::min(column, clock_room) : 0;
    const int clock_h = dials ? dial : ui::line_height(face);
    const int block = name_h + line_gap + clock_h + (with_offset ? line_gap + name_h : 0);
    const int top = std::max(0, (height_ - block) / 2);
    for (int i = 0; i < count_; ++i) {
      Place &p = places_[i];
      const int x = i * (column + gap);
      p.name_label = ui::label(parent, Font::BODY, theme::MUTED);
      lv_obj_set_style_text_align(p.name_label, LV_TEXT_ALIGN_CENTER, 0);
      lv_obj_set_size(p.name_label, column, name_h);
      lv_obj_set_pos(p.name_label, x, top);
      ui::set_text(p.name_label, p.name);
      const int clock_y = top + name_h + line_gap;
      if (dials) make_dial(parent, p, x + column / 2, clock_y + dial / 2, dial, large);
      else {
        p.time = ui::label(parent, face, theme::INK);
        lv_obj_set_style_text_align(p.time, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_size(p.time, column, clock_h);
        lv_obj_set_pos(p.time, x, clock_y);
      }
      if (with_offset) {
        p.offset = ui::label(parent, Font::BODY, theme::MUTED);
        lv_obj_set_style_text_align(p.offset, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_size(p.offset, column, name_h);
        lv_obj_set_pos(p.offset, x, clock_y + clock_h + line_gap);
      }
      show(p, false);
    }
    on_theme();
  }

  // A dial as Tessera's own analog clock has it: twelve marks, longer at the quarters, or numerals there where the
  // ring has room for them; the hour hand half the radius long, the minute hand most of it, a thin red second hand with
  // a short tail, and a dot in the middle.
  void make_dial(lv_obj_t *parent, Place &p, int cx, int cy, int dial, bool large) {
    p.dial = true;
    p.cx = cx;
    p.cy = cy;
    const int outer = dial / 2 - 1;
    p.radius = dial / 2 - ui::px(large ? 4 : 2);
    const int numeral_box = ui::line_height(Font::BODY) + 4, numeral_ring = outer - 11;
    const bool numerals = large && numeral_ring >= numeral_box + ui::px(2);
    for (int i = 0; i < 12; ++i) {
      const float a = i * PI / 6;
      const bool cardinal = i % 3 == 0;
      if (cardinal && numerals) {
        lv_obj_t *n = ui::label(parent, Font::BODY, theme::MUTED);
        lv_obj_set_style_text_align(n, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_size(n, numeral_box, numeral_box);
        lv_obj_set_pos(n, cx + std::lround(numeral_ring * sinf(a)) - numeral_box / 2,
                       cy - std::lround(numeral_ring * cosf(a)) - numeral_box / 2 + (numeral_box - ui::line_height(Font::BODY)) / 2);
        ui::set_text(n, i == 0 ? std::string("12") : std::to_string(i));
        p.numerals[i / 3] = n;
        continue;
      }
      const int length = cardinal ? ui::px(large ? 9 : 5) : ui::px(large ? 5 : 3);
      p.mark_points[i][0] = {static_cast<lv_value_precise_t>(cx + outer * sinf(a)), static_cast<lv_value_precise_t>(cy - outer * cosf(a))};
      p.mark_points[i][1] = {static_cast<lv_value_precise_t>(cx + (outer - length) * sinf(a)),
                             static_cast<lv_value_precise_t>(cy - (outer - length) * cosf(a))};
      p.marks[i] = line(parent, cardinal ? ui::px(large ? 3 : 2) : ui::px(large ? 2 : 1));
      lv_line_set_points(p.marks[i], p.mark_points[i], 2);
    }
    p.hour = line(parent, ui::px(large ? 5 : 3));
    p.minute = line(parent, ui::px(large ? 3 : 2));
    if (seconds_) p.second = line(parent, ui::px(large ? 2 : 1));
    const int centre = ui::px(large ? 8 : 4);
    p.dot = ui::block(parent, theme::ACCENT);
    lv_obj_set_style_radius(p.dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(p.dot, centre, centre);
    lv_obj_set_pos(p.dot, cx - centre / 2, cy - centre / 2);
  }

  static std::string digits_as_zero(std::string text) {
    for (char &ch : text)
      if (ch >= '1' && ch <= '9') ch = '0';
    return text;
  }

  const tessera::Plugin *plugin_;
  int width_ = 0, height_ = 0, count_ = 0;
  bool analog_ = true, seconds_ = true;
  Place places_[MAX_CLOCKS];
  lv_obj_t *note_{};
};

void WorldClock::setup() {
  add_tile("clocks", [this]() { return new ClocksTile(this); });
}

}  // namespace esphome::world_clock
