#include "main.h"
#include "WebServer.h"
#include <Arduino.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <WiFi.h>
/*----------------------------------------------------------------------------------------------
 * COMMUNICATION CLASS
 *--------------------------------------------------------------------------------------------*/
AsyncWebServer WebServer::server = AsyncWebServer(80);
WebSocketsServer WebServer::webSocket = WebSocketsServer(1337);

void WebServer::begin() {
  // Start file system
  if (!SPIFFS.begin()) {
    Serial.println("Error mounting SPIFFS");
  }

  // Start wifi
  Serial.print("Starting Wifi");
  WiFi.begin(config.network.ssid, config.network.password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(config.network.ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start MDNS
  if (MDNS.begin(config.network.hostname), WiFi.localIP()) {
    Serial.print("MDNS responder started. Hostname = ");
    Serial.println(config.network.hostname);
  }
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

  // On HTTP request for root, provide index.html file
  server.on("*", HTTP_GET, onIndexRequest);
  // Start web server
  server.begin();
  // Start WebSocket server and assign callback
  webSocket.onEvent(onWebSocketEvent);
  // Start web socket
  webSocket.begin();
}

void WebServer::update() {
  // Handle WebSocket data
  webSocket.loop();
}

void WebServer::onIndexRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
  if (request->url().equals("/"))
    request->send(SPIFFS, "/index.html", "text/html");
  else if (request->url().equals("/gui.json")) {
    String buffer;
    config.serialize(buffer);
    request->send(200, "application/json", buffer);
  } else if (SPIFFS.exists(request->url()))
    if (request->url().endsWith(".html")) {
      request->send(SPIFFS, request->url(), "text/html");
    } else if (request->url().endsWith(".css")) {
      request->send(SPIFFS, request->url(), "text/css");
    } else if (request->url().endsWith(".json")) {
      request->send(SPIFFS, request->url(), "application/json");
    } else if (request->url().endsWith(".ico")) {
      request->send(SPIFFS, request->url(), "image/x-icon");
    } else if (request->url().endsWith(".png")) {
      request->send(SPIFFS, request->url(), "image/png");
    } else {
      request->send(404, "text/plain", "Mime-Type Not Found");
    }
  else
    request->send(404, "text/plain", "Not Found");
}

void WebServer::onWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload,
                                 size_t length) {
  switch (type) {
    // Client has disconnected
    case WStype_DISCONNECTED: {
      Serial.printf("Disconnected client %u\n", client_num);
    } break;
    // New client has connected
    case WStype_CONNECTED: {
      Serial.printf("Connection from client %u\n", client_num);
    } break;
    // Text received from a connected client
    case WStype_TEXT: {
      config.execute(payload);
    } break;
    case WStype_BIN:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    case WStype_ERROR:
    case WStype_PING:
    case WStype_PONG:
      break;
  }
}