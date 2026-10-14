#pragma once
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/smart_display/plugin_api.h"

namespace esphome::world_clock {

// The plugin: one tile type ("clocks") with the time in up to three places of the world, and the switch that shows
// digits instead of dials on every clock of the screen (plugin.yaml).
class WorldClock : public Component, public tessera::Plugin {
 public:
  void setup() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  bool settings(tessera::SettingsPage &page) override;

  void set_digits(switch_::Switch *s) { digits_ = s; }
  // Whether the clocks show digits instead of dials, and a tap's turn of it.
  bool digits() const { return digits_ && digits_->state; }
  void turn_over() const {
    if (digits_) digits_->toggle();
  }

 protected:
  switch_::Switch *digits_{};
};

}  // namespace esphome::world_clock
