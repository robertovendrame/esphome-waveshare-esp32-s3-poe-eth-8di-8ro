#include "event_history.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include "esp_system.h"
#include "nvs_flash.h"

#include "esphome/core/log.h"

namespace esphome::event_history {

static const char *const TAG = "event_history";

void EventHistory::setup() {
  esp_err_t err = nvs_flash_init_partition(this->partition_.c_str());
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_LOGW(TAG, "Partizione non inizializzabile, la formatto");
    nvs_flash_erase_partition(this->partition_.c_str());
    err = nvs_flash_init_partition(this->partition_.c_str());
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Inizializzazione NVS fallita: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  err = nvs_open_from_partition(this->partition_.c_str(), "events", NVS_READWRITE, &this->handle_);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Apertura registro fallita: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  size_t size = sizeof(this->meta_);
  err = nvs_get_blob(this->handle_, "meta", &this->meta_, &size);
  if (err != ESP_OK || size != sizeof(this->meta_) || this->meta_.magic != META_MAGIC ||
      this->meta_.version != FORMAT_VERSION || this->meta_.capacity != this->max_events_ ||
      this->meta_.head >= this->max_events_ || this->meta_.count > this->max_events_) {
    this->meta_ = {META_MAGIC, FORMAT_VERSION, 0, 0, this->max_events_, 0};
    this->write_meta_();
  }

  this->ready_ = true;
  web_server_base::global_web_server_base->add_handler(this);
  ESP_LOGI(TAG, "Registro pronto: %u/%u eventi", this->meta_.count, this->max_events_);
}

void EventHistory::dump_config() {
  ESP_LOGCONFIG(TAG, "Registro eventi persistente:");
  ESP_LOGCONFIG(TAG, "  Partizione: %s", this->partition_.c_str());
  ESP_LOGCONFIG(TAG, "  Eventi: %u/%u", this->meta_.count, this->max_events_);
  ESP_LOGCONFIG(TAG, "  Pagina: /history");
}

void EventHistory::log_event(const std::string &category, const std::string &message) {
  if (!this->ready_)
    return;

  EventRecord record{};
  record.magic = RECORD_MAGIC;
  record.sequence = ++this->meta_.sequence;
  auto now = this->time_ != nullptr ? this->time_->now() : ESPTime{};
  record.timestamp = now.is_valid() ? now.timestamp : 0;
  record.uptime_seconds = millis() / 1000;
  std::snprintf(record.category, sizeof(record.category), "%s", category.c_str());
  std::snprintf(record.message, sizeof(record.message), "%s", message.c_str());

  const std::string key = record_key_(this->meta_.head);
  esp_err_t err = nvs_set_blob(this->handle_, key.c_str(), &record, sizeof(record));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Scrittura evento fallita: %s", esp_err_to_name(err));
    return;
  }

  this->meta_.head = (this->meta_.head + 1) % this->max_events_;
  this->meta_.count = std::min<uint16_t>(this->meta_.count + 1, this->max_events_);
  err = nvs_set_blob(this->handle_, "meta", &this->meta_, sizeof(this->meta_));
  if (err == ESP_OK)
    err = nvs_commit(this->handle_);
  if (err != ESP_OK)
    ESP_LOGE(TAG, "Commit evento fallito: %s", esp_err_to_name(err));
  else
    ESP_LOGI(TAG, "[%s] %s", record.category, record.message);
}

void EventHistory::log_boot() {
  this->log_event("SISTEMA", std::string("Avvio - causa: ") + reset_reason_());
}

void EventHistory::clear() {
  if (!this->ready_)
    return;
  nvs_erase_all(this->handle_);
  this->meta_ = {META_MAGIC, FORMAT_VERSION, 0, 0, this->max_events_, 0};
  this->write_meta_();
  ESP_LOGI(TAG, "Registro eventi cancellato");
}

bool EventHistory::canHandle(AsyncWebServerRequest *request) const {
  const std::string url = request->url();
  if (request->method() == HTTP_GET)
    return url == "/history" || url == "/history.csv" || url == "/history.json";
  return request->method() == HTTP_POST && url == "/history/clear";
}

void EventHistory::handleRequest(AsyncWebServerRequest *request) {
  const std::string url = request->url();
  if (request->method() == HTTP_POST && url == "/history/clear") {
    this->clear();
    request->redirect("/history");
    return;
  }

  AsyncWebServerResponse *response;
  if (url == "/history.csv") {
    response = request->beginResponse(200, "text/csv; charset=utf-8", this->render_csv_());
    response->addHeader("Content-Disposition", "attachment; filename=alarm-events.csv");
  } else if (url == "/history.json") {
    response = request->beginResponse(200, "application/json; charset=utf-8", this->render_json_());
  } else {
    response = request->beginResponse(200, "text/html; charset=utf-8", this->render_html_());
  }
  response->addHeader("Cache-Control", "no-store");
  request->send(response);
}

bool EventHistory::read_record_(uint16_t index, EventRecord *record) const {
  size_t size = sizeof(*record);
  const std::string key = record_key_(index);
  return nvs_get_blob(this->handle_, key.c_str(), record, &size) == ESP_OK && size == sizeof(*record) &&
         record->magic == RECORD_MAGIC;
}

bool EventHistory::write_meta_() {
  esp_err_t err = nvs_set_blob(this->handle_, "meta", &this->meta_, sizeof(this->meta_));
  if (err == ESP_OK)
    err = nvs_commit(this->handle_);
  return err == ESP_OK;
}

std::string EventHistory::format_time_(const EventRecord &record) const {
  if (record.timestamp <= 0) {
    char value[32];
    std::snprintf(value, sizeof(value), "Ora non disponibile (+%us)", record.uptime_seconds);
    return value;
  }
  auto value = ESPTime::from_epoch_local(record.timestamp);
  return value.strftime("%d/%m/%Y %H:%M:%S");
}

std::string EventHistory::render_html_() const {
  std::string out;
  out.reserve(8192 + this->meta_.count * 180);
  out += "<!doctype html><html lang='it'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  out += "<title>Storico eventi</title><style>body{font-family:system-ui;background:#111827;color:#e5e7eb;margin:0;padding:20px}main{max-width:1100px;margin:auto}h1{margin-bottom:4px}.sub{color:#9ca3af}a,button{background:#2563eb;color:white;border:0;border-radius:8px;padding:10px 14px;text-decoration:none;cursor:pointer}button{background:#b91c1c}nav{display:flex;gap:10px;flex-wrap:wrap;margin:20px 0}table{width:100%;border-collapse:collapse;background:#1f2937}th,td{text-align:left;padding:9px;border-bottom:1px solid #374151}th{position:sticky;top:0;background:#111827}tr:hover{background:#263244}.seq{color:#9ca3af}form{display:inline}@media(max-width:700px){th:nth-child(1),td:nth-child(1){display:none}body{padding:10px}th,td{font-size:13px;padding:7px}}</style></head><body><main>";
  out += "<h1>Storico eventi</h1><div class='sub'>Registro persistente circolare: " + std::to_string(this->meta_.count) + "/" + std::to_string(this->max_events_) + " eventi</div>";
  out += "<nav><a href='/'>Dispositivo</a><a href='/history.csv'>Scarica CSV</a><a href='/history.json'>JSON</a><form method='post' action='/history/clear' onsubmit=\"return confirm('Cancellare tutto lo storico?')\"><button type='submit'>Cancella storico</button></form></nav>";
  out += "<table><thead><tr><th>#</th><th>Data e ora</th><th>Categoria</th><th>Evento</th></tr></thead><tbody>";
  for (uint16_t pos = 0; pos < this->meta_.count; pos++) {
    const uint16_t index = (this->meta_.head + this->max_events_ - 1 - pos) % this->max_events_;
    EventRecord record{};
    if (!this->read_record_(index, &record))
      continue;
    out += "<tr><td class='seq'>" + std::to_string(record.sequence) + "</td><td>" + html_escape_(this->format_time_(record).c_str()) + "</td><td>" + html_escape_(record.category) + "</td><td>" + html_escape_(record.message) + "</td></tr>";
  }
  out += "</tbody></table></main></body></html>";
  return out;
}

std::string EventHistory::render_csv_() const {
  std::string out = "sequence;timestamp;category;event\r\n";
  out.reserve(64 + this->meta_.count * 150);
  const uint16_t oldest = (this->meta_.head + this->max_events_ - this->meta_.count) % this->max_events_;
  for (uint16_t pos = 0; pos < this->meta_.count; pos++) {
    EventRecord record{};
    if (!this->read_record_((oldest + pos) % this->max_events_, &record))
      continue;
    out += std::to_string(record.sequence) + ";" + csv_escape_(this->format_time_(record).c_str()) + ";" +
           csv_escape_(record.category) + ";" + csv_escape_(record.message) + "\r\n";
  }
  return out;
}

std::string EventHistory::render_json_() const {
  std::string out = "[";
  out.reserve(32 + this->meta_.count * 180);
  bool first = true;
  const uint16_t oldest = (this->meta_.head + this->max_events_ - this->meta_.count) % this->max_events_;
  for (uint16_t pos = 0; pos < this->meta_.count; pos++) {
    EventRecord record{};
    if (!this->read_record_((oldest + pos) % this->max_events_, &record))
      continue;
    if (!first)
      out += ',';
    first = false;
    out += "{\"sequence\":" + std::to_string(record.sequence) + ",\"timestamp\":\"" +
           json_escape_(this->format_time_(record).c_str()) + "\",\"category\":\"" +
           json_escape_(record.category) + "\",\"event\":\"" + json_escape_(record.message) + "\"}";
  }
  out += "]";
  return out;
}

std::string EventHistory::html_escape_(const char *value) {
  std::string out;
  for (; *value; value++) {
    switch (*value) {
      case '&': out += "&amp;"; break;
      case '<': out += "&lt;"; break;
      case '>': out += "&gt;"; break;
      case '\"': out += "&quot;"; break;
      case '\'': out += "&#39;"; break;
      default: out += *value;
    }
  }
  return out;
}

std::string EventHistory::csv_escape_(const char *value) {
  std::string out = "\"";
  for (; *value; value++) {
    if (*value == '\"')
      out += "\"\"";
    else
      out += *value;
  }
  out += '\"';
  return out;
}

std::string EventHistory::json_escape_(const char *value) {
  std::string out;
  for (; *value; value++) {
    switch (*value) {
      case '\\': out += "\\\\"; break;
      case '\"': out += "\\\""; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default: out += *value;
    }
  }
  return out;
}

const char *EventHistory::reset_reason_() {
  switch (esp_reset_reason()) {
    case ESP_RST_POWERON: return "alimentazione";
    case ESP_RST_SW: return "software";
    case ESP_RST_PANIC: return "crash/panic";
    case ESP_RST_INT_WDT: return "watchdog interrupt";
    case ESP_RST_TASK_WDT: return "watchdog task";
    case ESP_RST_WDT: return "watchdog";
    case ESP_RST_DEEPSLEEP: return "deep sleep";
    case ESP_RST_BROWNOUT: return "tensione insufficiente";
    case ESP_RST_SDIO: return "SDIO";
    case ESP_RST_USB: return "USB";
    case ESP_RST_JTAG: return "JTAG";
    default: return "sconosciuta";
  }
}

std::string EventHistory::record_key_(uint16_t index) {
  char key[8];
  std::snprintf(key, sizeof(key), "e%04u", index);
  return key;
}

}  // namespace esphome::event_history
