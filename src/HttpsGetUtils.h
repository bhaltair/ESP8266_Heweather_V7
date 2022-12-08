#ifndef _HTTPS_GET_UTILS_H_
#define _HTTPS_GET_UTILS_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#define DEBUG // 调试用

class HttpsGetUtils {
  public:
    HttpsGetUtils();

    void config(String userKey, String location, String unit, String lang);
    bool get(const char* url);
    String getServerCode();

  private:
    // const char* _host = "devapi.heweather.net"; // 服务器地址
    const char* _host = "api.heweather.local"; // 代理服务器地址
    const int httpsPort = 2053;

    String _requserKey;  // 私钥
    String _reqLocation; // 位置
    String _reqUnit;     // 单位
    String _reqLang;     // 语言


    String _response_code =  "no_init";  // API状态码
};

#endif
