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
    bool getString(const char* url, uint8_t*& outbuf, size_t &len);


  private:
    bool fetchBuffer(const char* url);
    // const char* _host = "devapi.heweather.net"; // 服务器地址
    const char* _host = "api.heweather.local"; // 代理服务器地址
    const int httpsPort = 2053;


    String _requserKey;  // 私钥
    String _reqLocation; // 位置
    String _reqUnit;     // 单位
    String _reqLang;     // 语言
    
    uint8_t _buffer[1280]; //gzip流最大缓冲区
    size_t _bufferSize=0;

};

#endif
