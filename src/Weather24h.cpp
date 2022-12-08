#include "Weather24h.h"

Weather24h::Weather24h() {
}

void Weather24h::config(String userKey, String location, String unit, String lang) {
  _requserKey = userKey;
  _reqLocation = location;
  _reqUnit = unit;
  _reqLang = lang;
}

bool Weather24h::get() {
  // http请求
    WiFiClient client;
    HTTPClient http;
  #ifdef DEBUG
  Serial.print("[HTTP] begin...\n");
  #endif DEBUG
  String api = "http://192.168.2.180:8081";
  String url = api + "/v7/weather/3d?location=" + _reqLocation +
              "&key=" + _requserKey + "&unit=" + _reqUnit + "&lang=" + _reqLang;// + "&gzip=n";
          url="http://192.168.2.144:8082";
  if (http.begin(client, url)) {  // HTTP连接成功
    #ifdef DEBUG
    Serial.print("[HTTP] GET...\n");
    #endif DEBUG
    int httpCode = http.GET(); // 请求

    if (httpCode > 0) { // 错误返回负值
      #ifdef DEBUG
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      #endif DEBUG
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { // 服务器响应
        String payload = http.getString();
        #ifdef DEBUG
        Serial.println(payload);
        #endif DEBUG
        _parseNowJson(payload);
        return true;
      }
    } else { // 错误返回负值
      #ifdef DEBUG
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      #endif DEBUG
      return false;
    }
    http.end();
  } else { // HTTP连接失败
    #ifdef DEBUG
    Serial.printf("[HTTP] Unable to connect\n");
    #endif DEBUG
    return false;
  }
}

void Weather24h::_parseNowJson(String payload) {
  // std::string input;

DynamicJsonDocument doc(8192);

DeserializationError error = deserializeJson(doc, payload);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

_response_code = doc["code"].as<String>(); // "200"
_last_update_str  = doc["updateTime"].as<String>(); // "2022-12-05T15:35+08:00"
// const char* fxLink = doc["fxLink"]; // "http://hfx.link/1u0r1"

short i=0;
for (JsonObject hourly_item : doc["hourly"].as<JsonArray>()) {
 _hourly_item_fxTime[i] = hourly_item["fxTime"].as<String>(); // "2022-12-05T17:00+08:00", ...
 _hourly_item_temp_int[i] = hourly_item["temp"].as<int>(); // "15", "13", "13", "12", "11", "11", "10", "10", ...
  _hourly_item_icon_int[i] = hourly_item["icon"].as<int>(); // "100", "150", "150", "150", "150", "150", "150", ...
  _hourly_item_text[i] = hourly_item["text"].as<String>(); // "晴", "晴", "晴", "晴", "晴", "晴", "晴", "多云", "多云", ...
  // const char* hourly_item_wind360 = hourly_item["wind360"]; // "22", "24", "30", "33", "33", "31", "30", ...
  // const char* hourly_item_windDir = hourly_item["windDir"]; // "东北风", "东北风", "东北风", "东北风", "东北风", "东北风", ...
  // const char* hourly_item_windScale = hourly_item["windScale"]; // "3-4", "3-4", "3-4", "3-4", "3-4", ...
  // const char* hourly_item_windSpeed = hourly_item["windSpeed"]; // "16", "16", "16", "16", "14", "14", ...
  _hourly_item_humidity_int[i] = hourly_item["humidity"].as<int>(); // "57", "63", "63", "65", "66", "67", "68", ...
  // const char* hourly_item_pop = hourly_item["pop"]; // "1", "3", "6", "6", "6", "6", "6", "6", "7", "7", ...
  _hourly_item_precip_float[i] = hourly_item["precip"].as<float>(); // "0.0", "0.0", "0.0", "0.0", "0.0", "0.0", ...
  // const char* hourly_item_pressure = hourly_item["pressure"]; // "1013", "1013", "1012", "1012", "1012", ...
  // const char* hourly_item_cloud = hourly_item["cloud"]; // "5", "5", "4", "4", "7", "9", "11", "33", "54", ...
  // const char* hourly_item_dew = hourly_item["dew"]; // "7", "6", "6", "6", "5", "5", "5", "5", "5", "4", ...
  i++;
}

// JsonArray refer_sources = doc["refer"]["sources"];
// const char* refer_sources_0 = refer_sources[0]; // "QWeather"
// const char* refer_sources_1 = refer_sources[1]; // "NMC"
// const char* refer_sources_2 = refer_sources[2]; // "ECMWF"

// const char* refer_license_0 = doc["refer"]["license"][0]; // "CC BY-SA 4.0"

}

// 返回API状态码
String Weather24h::getServerCode() {
  return _response_code;
}

// 返回当前API最近更新时间
String Weather24h::getLastUpdate() {
  return _last_update_str;
}

String Weather24h::getFxDate(int index) {
  return _hourly_item_fxTime[index];
}

int Weather24h::getTemp(int index) {
  return _hourly_item_temp_int[index];
}
int Weather24h::getIconDay(int index){
  return _hourly_item_icon_int[index];
}
String Weather24h::getTextDay(int index) {
  return _hourly_item_text[index];
}
int Weather24h::getHumidity(int index) {
  return _hourly_item_humidity_int[index];
}
float Weather24h::getPrecip(int index) {
  return _hourly_item_precip_float[index];
}