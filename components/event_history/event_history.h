#pragma once

#include <cstdint>
#include <string>

#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include "esphome/core/component.h"

#include "nvs.h"

namespace esphome::event_history {

class EventHistory : public Component, public AsyncWebHandler {
 public:
  void set_time_source(time::RealTimeClock *time) { this->time_ = time; }
  void set_max_events(uint16_t max_events) { this->max_events_ = max_events; }
  void set_partition(const std::string &partition) { this->partition_ = partition; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void log_event(const std::string &category, const std::string &message);
  void log_boot();
  void clear();
  uint16_t size() const { return this->meta_.count; }

  bool canHandle(AsyncWebServerRequest *request) const override;
  void handleRequest(AsyncWebServerRequest *request) override;

 protected:
  static constexpr uint32_t META_MAGIC = 0x45564D31;
  static constexpr uint32_t RECORD_MAGIC = 0x45565231;
  static constexpr uint16_t FORMAT_VERSION = 1;

  struct Metadata {
    uint32_t magic;
    uint16_t version;
    uint16_t head;
    uint16_t count;
    uint16_t capacity;
    uint32_t sequence;
  };

  struct EventRecord {
    uint32_t magic;
    uint32_t sequence;
    int64_t timestamp;
    uint32_t uptime_seconds;
    char category[24];
    char message[96];
  };

  bool read_record_(uint16_t index, EventRecord *record) const;
  bool write_meta_();
  std::string format_time_(const EventRecord &record) const;
  std::string render_html_() const;
  std::string render_csv_() const;
  std::string render_json_() const;
  static std::string html_escape_(const char *value);
  static std::string csv_escape_(const char *value);
  static std::string json_escape_(const char *value);
  static const char *reset_reason_();
  static std::string record_key_(uint16_t index);

  time::RealTimeClock *time_{nullptr};
  std::string partition_{"event_log"};
  uint16_t max_events_{500};
  nvs_handle_t handle_{0};
  bool ready_{false};
  Metadata meta_{META_MAGIC, FORMAT_VERSION, 0, 0, 500, 0};
};

}  // namespace esphome::event_history

