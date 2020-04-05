#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>
#include <Arduino.h>
#include "ArduinoJson.h"
#include <SPIFFS.h>
#include "Animation.h"

#define COMMAND_DOC_SIZE 255
#define CONFIG_DOC_SIZE 4095
/*---------------------------------------------------------------------------------------
 * Evil global Config parameters
 *
 * Animation init or draw routines need to apply config parameters to dynamically set
 * runtime parameters. Init only gets called when an animation starts or restarts
 * draw gets called every animation frame so choose wisely where to apply.
 *
 * After creation of the config object, call load() to load the configuration from
 * the "config.json" file and apply the values to the config struct.
 *
 * If no "config.json" exists the config structs keeps the values supplied in the struct
 * after saveing a "config.json" is created.
 *-------------------------------------------------------------------------------------*/
struct Config {
  struct {
    uint16_t max_milliamps = 8000;
    bool gamma_correct = false;
  } display;
  struct {
    char ssid[32] = "-^..^-";
    char password[64] = "";
    char hostname[64] = "";
  } network;
  struct {
    uint8_t brightness = 255;
    uint8_t red_energy = 180;
  } fire;
  struct {
    float timer = 10.0f;
    uint8_t brightness = 70;
    bool dynamic_noise = true;
    float scale_p = 0.15f;
    float speed_x = 0.1f;
    float speed_y = 0.2f;
    float speed_z = 0.3f;
  } noise;
  struct {
    float timer = 10.0f;
    uint8_t brightness = 100;
    uint8_t pixel_density = 64;
    uint8_t fade_in_speed = 8;
    uint8_t fade_out_speed = 4;
  } twinkels;

  File openFile(const char* name, const char* mode) {
    if (!SPIFFS.begin()) {
      Serial.println("Error mounting SPIFFS");
      return (File)0;
    }
    File file = SPIFFS.open(name, mode);
    if (!file) {
      Serial.printf("Error opening file for %s\n", mode);
      return (File)0;
    }
    return file;
  }

  // Load config from file system
  void load() {
    if (File file = openFile("/config.json", FILE_READ)) {
      String buffer = file.readString();
      Serial.printf("%u bytes read from config.json\n", buffer.length());
      file.close();
      deserialize(buffer);
    }
  }

  // Save config to file system
  void save() {
    if (File file = openFile("/config.json", FILE_WRITE)) {
      String buffer;
      serialize(buffer);
      int bytesWritten = file.print(buffer);
      file.close();
      Serial.printf("%u bytes written to config.json\n", bytesWritten);
    }
  }

  // Serializes config and creates gui json
  void serialize(String& buffer) {
    DynamicJsonDocument doc(CONFIG_DOC_SIZE);

    JsonObject settings = doc.createNestedObject("settings");
    settings["name"] = "Configuration Settings";

    JsonObject settings_display = settings.createNestedObject("display");
    settings_display["name"] = "Display Settings";

    JsonObject settings_display_max_milliamps =
        settings_display.createNestedObject("max_milliamps");
    settings_display_max_milliamps["name"] = "Max mAmps";
    settings_display_max_milliamps["type"] = "slider";
    settings_display_max_milliamps["value"] = display.max_milliamps;
    settings_display_max_milliamps["min"] = 0;
    settings_display_max_milliamps["max"] = 10000;
    settings_display_max_milliamps["step"] = 100;

    JsonObject settings_display_gamma_correct =
        settings_display.createNestedObject("gamma_correct");
    settings_display_gamma_correct["name"] = "Gamma Correction";
    settings_display_gamma_correct["type"] = "checkbox";
    settings_display_gamma_correct["value"] = display.gamma_correct;

    JsonObject settings_network = settings.createNestedObject("network");
    settings_network["name"] = "Network Settings";

    JsonObject settings_network_ssid = settings_network.createNestedObject("ssid");
    settings_network_ssid["name"] = "Network SSID";
    settings_network_ssid["type"] = "text";
    settings_network_ssid["value"] = network.ssid;
    settings_network_ssid["size"] = 32;

    JsonObject settings_network_password =
        settings_network.createNestedObject("password");
    settings_network_password["name"] = "Password";
    settings_network_password["type"] = "text";
    settings_network_password["value"] = network.password;
    settings_network_password["size"] = 64;

    JsonObject settings_network_hostname =
        settings_network.createNestedObject("hostname");
    settings_network_hostname["name"] = "Hostname";
    settings_network_hostname["type"] = "text";
    settings_network_hostname["value"] = network.hostname;
    settings_network_hostname["size"] = 64;

    JsonObject animations = doc.createNestedObject("animations");
    animations["name"] = "Animation Settings";

    JsonObject animations_fire = animations.createNestedObject("fire");
    animations_fire["name"] = "Burning Fire";
    animations_fire["index"] = 0;

    JsonObject animations_fire_brightness =
        animations_fire.createNestedObject("brightness");
    animations_fire_brightness["name"] = "Brightness";
    animations_fire_brightness["type"] = "slider";
    animations_fire_brightness["value"] = fire.brightness;
    animations_fire_brightness["min"] = 0;
    animations_fire_brightness["max"] = 255;
    animations_fire_brightness["step"] = 1;

    JsonObject animations_fire_red_energy =
        animations_fire.createNestedObject("red_energy");
    animations_fire_red_energy["name"] = "Red Energy";
    animations_fire_red_energy["type"] = "slider";
    animations_fire_red_energy["value"] = fire.red_energy;
    animations_fire_red_energy["min"] = 0;
    animations_fire_red_energy["max"] = 255;
    animations_fire_red_energy["step"] = 1;

    JsonObject animations_noise = animations.createNestedObject("noise");
    animations_noise["name"] = "Dynamic Noise";
    animations_noise["index"] = 1;

    JsonObject animations_noise_timer = animations_noise.createNestedObject("timer");
    animations_noise_timer["name"] = "Timer";
    animations_noise_timer["type"] = "slider";
    animations_noise_timer["value"] = noise.timer;
    animations_noise_timer["min"] = 0;
    animations_noise_timer["max"] = 60;
    animations_noise_timer["step"] = 1;

    JsonObject animations_noise_brightness =
        animations_noise.createNestedObject("brightness");
    animations_noise_brightness["name"] = "Brightness";
    animations_noise_brightness["type"] = "slider";
    animations_noise_brightness["value"] = noise.brightness;
    animations_noise_brightness["min"] = 0;
    animations_noise_brightness["max"] = 255;
    animations_noise_brightness["step"] = 1;

    JsonObject animations_noise_scale_p = animations_noise.createNestedObject("scale_p");
    animations_noise_scale_p["name"] = "Scale";
    animations_noise_scale_p["type"] = "slider";
    animations_noise_scale_p["value"] = noise.scale_p;
    animations_noise_scale_p["min"] = -3;
    animations_noise_scale_p["max"] = 3;
    animations_noise_scale_p["step"] = 0.01;

    JsonObject animations_noise_dynamic_noise =
        animations_noise.createNestedObject("dynamic_noise");
    animations_noise_dynamic_noise["name"] = "Dynamic Noise";
    animations_noise_dynamic_noise["type"] = "checkbox";
    animations_noise_dynamic_noise["value"] = noise.dynamic_noise;

    JsonObject animations_noise_speed_x = animations_noise.createNestedObject("speed_x");
    animations_noise_speed_x["name"] = "X-Speed";
    animations_noise_speed_x["type"] = "slider";
    animations_noise_speed_x["value"] = noise.speed_x;
    animations_noise_speed_x["min"] = -3;
    animations_noise_speed_x["max"] = 3;
    animations_noise_speed_x["step"] = 0.01;

    JsonObject animations_noise_speed_y = animations_noise.createNestedObject("speed_y");
    animations_noise_speed_y["name"] = "Y-Speed";
    animations_noise_speed_y["type"] = "slider";
    animations_noise_speed_y["value"] = noise.speed_y;
    animations_noise_speed_y["min"] = -3;
    animations_noise_speed_y["max"] = 3;
    animations_noise_speed_y["step"] = 0.01;

    JsonObject animations_noise_speed_z = animations_noise.createNestedObject("speed_z");
    animations_noise_speed_z["name"] = "Z-Speed";
    animations_noise_speed_z["type"] = "slider";
    animations_noise_speed_z["value"] = noise.speed_z;
    animations_noise_speed_z["min"] = -3;
    animations_noise_speed_z["max"] = 3;
    animations_noise_speed_z["step"] = 0.01;

    JsonObject animations_twinkels = animations.createNestedObject("twinkels");
    animations_twinkels["name"] = "Twinkeling twilight";
    animations_twinkels["index"] = 2;

    JsonObject animations_twinkels_timer =
        animations_twinkels.createNestedObject("timer");
    animations_twinkels_timer["name"] = "Timer";
    animations_twinkels_timer["type"] = "slider";
    animations_twinkels_timer["value"] = twinkels.timer;
    animations_twinkels_timer["min"] = 0;
    animations_twinkels_timer["max"] = 60;
    animations_twinkels_timer["step"] = 1;

    JsonObject animations_twinkels_brightness =
        animations_twinkels.createNestedObject("brightness");
    animations_twinkels_brightness["name"] = "Brightness";
    animations_twinkels_brightness["type"] = "slider";
    animations_twinkels_brightness["value"] = twinkels.brightness;
    animations_twinkels_brightness["min"] = 0;
    animations_twinkels_brightness["max"] = 255;
    animations_twinkels_brightness["step"] = 1;

    JsonObject animations_twinkels_pixel_density =
        animations_twinkels.createNestedObject("pixel_density");
    animations_twinkels_pixel_density["name"] = "Density";
    animations_twinkels_pixel_density["type"] = "slider";
    animations_twinkels_pixel_density["value"] = twinkels.pixel_density;
    animations_twinkels_pixel_density["min"] = 0;
    animations_twinkels_pixel_density["max"] = 255;
    animations_twinkels_pixel_density["step"] = 1;

    JsonObject animations_twinkels_fade_in_speed =
        animations_twinkels.createNestedObject("fade_in_speed");
    animations_twinkels_fade_in_speed["name"] = "Fade in speed";
    animations_twinkels_fade_in_speed["type"] = "slider";
    animations_twinkels_fade_in_speed["value"] = twinkels.fade_in_speed;
    animations_twinkels_fade_in_speed["min"] = 0;
    animations_twinkels_fade_in_speed["max"] = 255;
    animations_twinkels_fade_in_speed["step"] = 1;

    JsonObject animations_twinkels_fade_out_speed =
        animations_twinkels.createNestedObject("fade_out_speed");
    animations_twinkels_fade_out_speed["name"] = "Fade out speed";
    animations_twinkels_fade_out_speed["type"] = "slider";
    animations_twinkels_fade_out_speed["value"] = twinkels.fade_out_speed;
    animations_twinkels_fade_out_speed["min"] = 0;
    animations_twinkels_fade_out_speed["max"] = 255;
    animations_twinkels_fade_out_speed["step"] = 1;

    serializeJson(doc, buffer);
  }

  // Deserializes message from gui to config struct
  void deserialize(String buffer) {
    StaticJsonDocument<CONFIG_DOC_SIZE> doc;
    DeserializationError err = deserializeJson(doc, buffer);
    if (err) {
      Serial.printf("Deserialization error: %s\n", err.c_str());
      return;
    }
    /* ------------------------------ DISPLAY --------------------------------*/
    display.max_milliamps =
        doc["settings"]["display"]["max_milliamps"]["value"] | display.max_milliamps;
    display.gamma_correct =
        doc["settings"]["display"]["gamma_correct"]["value"] | display.gamma_correct;
    /* ------------------------------ NETWORK --------------------------------*/
    strlcpy(network.ssid, doc["settings"]["network"]["ssid"]["value"] | network.ssid,
            sizeof(network.ssid));
    strlcpy(network.hostname,
            doc["settings"]["network"]["hostname"]["value"] | network.hostname,
            sizeof(network.hostname));
    strlcpy(network.password,
            doc["settings"]["network"]["password"]["value"] | network.password,
            sizeof(network.password));
    /* ------------------------------ FIRE -----------------------------------*/
    fire.brightness = doc["animations"]["fire"]["brightness"]["value"] | fire.brightness;
    fire.red_energy = doc["animations"]["fire"]["red_energy"]["value"] | fire.red_energy;
    /* ------------------------------ NOISE ----------------------------------*/
    noise.timer = doc["animations"]["noise"]["timer"]["value"] | noise.timer;
    noise.brightness =
        doc["animations"]["noise"]["brightness"]["value"] | noise.brightness;
    noise.dynamic_noise =
        doc["animations"]["noise"]["dynamic_noise"]["value"] | noise.dynamic_noise;
    noise.scale_p = doc["animations"]["noise"]["scale_p"]["value"] | noise.scale_p;
    noise.speed_x = doc["animations"]["noise"]["speed_x"]["value"] | noise.speed_x;
    noise.speed_y = doc["animations"]["noise"]["speed_y"]["value"] | noise.speed_y;
    noise.speed_z = doc["animations"]["noise"]["speed_z"]["value"] | noise.speed_z;
    /* ------------------------------ TWINKELS --------------------------------*/
    twinkels.timer = doc["animations"]["twinkels"]["timer"]["value"] | twinkels.timer;
    twinkels.brightness =
        doc["animations"]["twinkels"]["brightness"]["value"] | twinkels.brightness;
    twinkels.fade_in_speed =
        doc["animations"]["twinkels"]["fade_in_speed"]["value"] | twinkels.fade_in_speed;
    twinkels.fade_out_speed = doc["animations"]["twinkels"]["fade_out_speed"]["value"] |
                              twinkels.fade_out_speed;
    twinkels.pixel_density =
        doc["animations"]["twinkels"]["pixel_density"]["value"] | twinkels.pixel_density;
  }

  // Executes json command send from the client
  void execute(uint8_t* buffer) {
    StaticJsonDocument<COMMAND_DOC_SIZE> doc;
    DeserializationError err = deserializeJson(doc, buffer);
    if (err) {
      Serial.printf("Deserialization error: %s\n", err.c_str());
      return;
    }
    serializeJson(doc, Serial);
    Serial.println();

    // Handle the event send from the gui
    String event = doc["event"];
    if (event.equals("activate")) {
      Animation::set(doc["target"]);
    } else if (event.equals("update")) {
      String buffer2;
      // event propery is not needed any more, remove from document
      doc.remove("event");
      // serialize again because deserializeJson destroys the buffer
      serializeJson(doc, buffer2);
      // deserialize the new buffer to the config struct
      deserialize(buffer2);
    } else if (event.equals("save")) {
      save();
    }
  }
};
extern struct Config config;
#endif