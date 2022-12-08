#include "HttpsGetUtils.h"
#include "ArduinoZlib.h"

HttpsGetUtils::HttpsGetUtils() {
}
void heap(){
  Serial.print("FREE_HEAP[");
  Serial.print(ESP.getFreeHeap());
  Serial.print("]\n");
}

void log(const char *str) {
  Serial.println(str);
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

bool HttpsGetUtils::fetchBuffer(const char *url) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    Serial.print("[HTTPS] begin...\n");
    HTTPClient https;
    if (https.begin(*client, url)) {
      https.addHeader("Accept-Encoding", "gzip");
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {

          // get length of document (is -1 when Server sends no Content-Length header)
          int len = https.getSize();

          // create buffer for read
          static uint8_t buff[16] = { 0 };

          // read all data from server
          int offset=0;
          //  为什么这里分配内存会报错？
          // if(inbuf==NULL) inbuf=(uint8_t*)malloc(sizeof(uint8_t)*128);
          while (https.connected() && (len > 0 || len == -1)) {
            // get available data size
            size_t size = client->available();
            
            if (size) {
              // read up to 128 byte
              int c = client->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
              // int c = client->readBytes(buff, size);
              // Serial.println("memcpy");
              memcpy(_buffer+offset, buff, sizeof(uint8_t)*c);
              offset+=c;
              if(c>0 && c!=16) {
                log("======rb====");
                Serial.printf("%d,", buff[c-3]);
                Serial.printf("%d,", buff[c-2]);
                Serial.printf("%d,", buff[c-1]);
                log("\n======rb end====");
              }
              // write it to Serial
              // Serial.write(buff, c);
              if (len > 0) {
                len -= c;
              } 
            }
            delay(1);
          }
          _bufferSize=offset;
          Serial.printf("offset=%d\n", offset);
          // Serial.write(_buffer, offset);
          Serial.print("[HTTPS] connection closed or file end.\n");
          
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();

    } else {
      Serial.printf("Unable to connect\n");
    }
    return _bufferSize>0;;
}

bool HttpsGetUtils::getString(const char* url, uint8_t *& outbuf, size_t &outlen) {
  fetchBuffer(url);
  Serial.printf("\nAfter fetch, buffer size=%d\n", _bufferSize);
  delay(1000);
  if(_bufferSize) {
    // write it to Serial
    log("===buf===");
    Serial.printf("%d,", _bufferSize-3);
    Serial.printf("%d,", _bufferSize-2);
    Serial.printf("%d,", _bufferSize-1);
    log("\n===buf end===");    
    Serial.write(_buffer,_bufferSize);
    outbuf=(uint8_t*)malloc(sizeof(uint8_t)*12000);
    if(outbuf==NULL) log("outbuf allocate failed!");
    uint32_t outprintsize=0;
    int result=ArduinoZlib::libmpq__decompress_zlib(_buffer, _bufferSize, outbuf, 12000,outprintsize);
    Serial.printf("outsize=%d, result=%d\n", outprintsize,result);
    // parseJSON((char*)outbuf, outprintsize);
    //Serial.write(outbuf,outprintsize);
    outlen=outprintsize;
    Serial.printf("outlen :%d\n", outlen);
    _bufferSize=0;
    return true;
  }else {
    Serial.println("no avali size!");
  }
  return false;
  // if (outbuf!=NULL){
  //   free(outbuf);
  //   outbuf=NULL;
  // }
}



