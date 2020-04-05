#ifndef COMM_H
#define COMM_H
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <DNSServer.h>
#include <WiFi.h>
/*----------------------------------------------------------------------------------------------
 * COMMUNICATION CLASS
 *--------------------------------------------------------------------------------------------*/
class WebServer {
 private:
  static AsyncWebServer server;
  static WebSocketsServer webSocket;
  static DNSServer dnsServer;
  static IPAddress APIP;
  static boolean AP_MODE;

 public:
  static void begin();
  static void update();

 private:
  static void onIndexRequest(AsyncWebServerRequest *request);
  static void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload,
                               size_t length);
};
#endif