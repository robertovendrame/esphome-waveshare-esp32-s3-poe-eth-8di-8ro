import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import time
from esphome.const import CONF_ID, CONF_TIME_ID

CODEOWNERS = ["@robertovendrame"]
DEPENDENCIES = ["esp32", "web_server"]

CONF_MAX_EVENTS = "max_events"
CONF_PARTITION = "partition"

event_history_ns = cg.esphome_ns.namespace("event_history")
EventHistory = event_history_ns.class_("EventHistory", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(EventHistory),
        cv.Required(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        cv.Optional(CONF_MAX_EVENTS, default=500): cv.int_range(min=10, max=1000),
        cv.Optional(CONF_PARTITION, default="event_log"): cv.string_strict,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    clock = await cg.get_variable(config[CONF_TIME_ID])
    cg.add(var.set_time_source(clock))
    cg.add(var.set_max_events(config[CONF_MAX_EVENTS]))
    cg.add(var.set_partition(config[CONF_PARTITION]))

