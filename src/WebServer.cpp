#include "main.h"
#include "WebServer.h"
#include <Arduino.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include "Animation.h"
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
    Serial.println("MDNS responder started");
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
  else if (SPIFFS.exists(request->url()))
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
      // Serial.printf("Received text from client %u %s\n", client_num, payload);
      executeCommand(payload);
      // webSocket.sendTXT(client_num, "hallo");
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

bool parse(const char *command, String &msg) {
  if (msg.startsWith(command)) {
    msg.remove(0, strlen(command));
    return true;
  }
  return false;
}

void WebServer::executeCommand(uint8_t *payload) {
  String msg = String((char *)payload);
  if (parse("save:", msg)) {
    // TODO
    return;
  }

  if (parse("activate:", msg)) {
    if (parse("animations.fire", msg)) {
      Animation::set(0);
      return;
    }
    if (parse("animations.noise", msg)) {
      Animation::set(1);
      return;
    }
    if (parse("animations.twinkels", msg)) {
      Animation::set(2);
      return;
    }
  }

  if (parse("update:", msg)) {
    /*---------------------------------------------
     * update: DISPLAY SETTINGS
     * -------------------------------------------*/
    if (parse("settings.display.max_milliamps=", msg)) {
      config.display.max_milliamps = msg.toInt();
      return;
    }
    if (parse("settings.display.gamma_correct=", msg)) {
      config.display.gamma_correct = msg.equals("true");
      return;
    }
    /*---------------------------------------------
     * update: NETWORK SETTINGS
     * -------------------------------------------*/
    if (parse("settings.network.ssid=", msg)) {
      // config.network.ssid
      return;
    }
    if (parse("settings.network.password=", msg)) {
      // config.network.password
      return;
    }
    if (parse("settings.network.hostname=", msg)) {
      // config.network.hostname
      return;
    }
    /*---------------------------------------------
     * update: FIRE SETTINGS
     * -------------------------------------------*/
    if (parse("animations.fire.brightness=", msg)) {
      config.fire.brightness = msg.toInt();
      return;
    }
    if (parse("animations.fire.red_energy=", msg)) {
      config.fire.brightness = msg.toInt();
      return;
    }
    /*---------------------------------------------
     * update: NOISE SETTINGS
     * -------------------------------------------*/
    if (parse("animations.noise.timer=", msg)) {
      config.noise.timer = msg.toInt();
      return;
    }
    if (parse("animations.noise.brightness=", msg)) {
      config.noise.brightness = msg.toInt();
      return;
    }
    if (parse("animations.noise.dynamic_noise=", msg)) {
      config.noise.dynamic_noise = msg.equals("true");
      return;
    }
    if (parse("animations.noise.scale_p=", msg)) {
      config.noise.scale_p = msg.toFloat();
      return;
    }
    if (parse("animations.noise.speed_x=", msg)) {
      config.noise.speed_x = msg.toFloat();
      return;
    }
    if (parse("animations.noise.speed_y=", msg)) {
      config.noise.speed_y = msg.toFloat();
      return;
    }
    if (parse("animations.noise.speed_z=", msg)) {
      config.noise.speed_z = msg.toFloat();
      return;
    }
    /*---------------------------------------------
     * update: TWINKELS SETTINGS
     * -------------------------------------------*/
    if (parse("animations.twinkels.timer=", msg)) {
      config.twinkels.timer = msg.toInt();
      return;
    }
    if (parse("animations.twinkels.brightness=", msg)) {
      config.twinkels.brightness = msg.toInt();
      return;
    }
    if (parse("animations.twinkels.pixel_density=", msg)) {
      config.twinkels.pixel_density = msg.toInt();
      return;
    }
    if (parse("animations.twinkels.fade_in_speed=", msg)) {
      config.twinkels.fade_in_speed = msg.toInt();
      return;
    }
    if (parse("animations.twinkels.fade_out_speed=", msg)) {
      config.twinkels.fade_out_speed = msg.toInt();
      return;
    }
  }
  Serial.println("Unknow command: " + msg);
}