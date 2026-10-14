"""World clock: the ESPHome side. smart_display.register_plugin() reads the id, version, tile memory and the
screen's texts from the manifest and translations/ beside this folder (docs/FIRMWARE_API.md of tessera-plugins).

`digits` is the switch of plugin.yaml that shows digits instead of dials on every clock of the screen."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import smart_display, switch
from esphome.const import CONF_ID

# The core component every Tessera screen has; the plugin API lives in it.
DEPENDENCIES = ["smart_display"]

CONF_DIGITS = "digits"

world_clock_ns = cg.esphome_ns.namespace("world_clock")
WorldClock = world_clock_ns.class_("WorldClock", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(WorldClock),
        cv.Optional(CONF_DIGITS): cv.use_id(switch.Switch),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await smart_display.register_plugin(var, __file__)
    if CONF_DIGITS in config:
        cg.add(var.set_digits(await cg.get_variable(config[CONF_DIGITS])))
