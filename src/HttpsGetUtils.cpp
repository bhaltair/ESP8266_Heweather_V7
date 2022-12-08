#include "HttpsGetUtils.h"

HttpsGetUtils::HttpsGetUtils() {
}

// 配置请求信息，私钥、位置、单位、语言
void HttpsGetUtils::config(String userKey, String location, String unit, String lang) {
  _requserKey = userKey;
  _reqLocation = location;
  _reqUnit = unit;
  _reqLang = lang;
}

// 尝试获取信息，成功返回true，失败返回false
bool HttpsGetUtils::get(const char *url) {
  // https请求
    WiFiClient client;
    HTTPClient http;

  #ifdef DEBUG
  Serial.print("[HTTP] begin...\n");
  #endif DEBUG
  String api = "http://192.168.2.180:8081";
  // String url = api + "/v7/weather/now?location=" + _reqLocation + "&key=" + _requserKey + "&unit=" + _reqUnit + "&lang=" + _reqLang;// + "&gzip=n";

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


// API状态码
String HttpsGetUtils::getServerCode() {
  return _response_code;
}
