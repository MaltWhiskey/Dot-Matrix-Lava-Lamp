#ifndef MYWEBSERVER_H
#define MYWEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

namespace WebServer {

File openFile(const char* name, const char* mode);
void begin();
void update(float temp);
void onIndexRequest(AsyncWebServerRequest* request);
void onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t* payload, size_t length);

}  // namespace WebServer
#endif