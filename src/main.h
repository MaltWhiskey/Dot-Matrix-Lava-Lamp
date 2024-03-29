#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <SPIFFS.h>
#include <stdint.h>

#include "Animation.h"
#include "ArduinoJson.h"

// This json document size has plenty of space to hold commands send from the
// gui.
#define COMMAND_DOC_SIZE 255
// This json document size may need to be modified if config gets bigger
#define CONFIG_DOC_SIZE 8192
/*---------------------------------------------------------------------------------------
 * Evil global Config parameters
 *
 * Animation init or draw routines need to apply config parameters to
 *dynamically set runtime parameters. Init only gets called when an animation
 *starts or restarts draw gets called every animation frame so choose wisely
 *where to apply.
 *
 * After creation of the config object, call load() to load the configuration
 *from the "config.json" file and apply the values to the config struct.
 *
 * If no "config.json" exists the config structs keeps the values supplied in
 *the struct After saveing a "config.json" is freshly created.
 *-------------------------------------------------------------------------------------*/
struct Config {
  struct {
    uint16_t max_milliamps = 8000;
    bool gamma_correct = false;
    float chart_timer = 0.1f;
    uint16_t chart_size = 500;
  } display;
  struct {
    char ssid[32] = "-^..^-";
    char password[64] = "qazwsxedc";
    char hostname[64] = "rechts";
    char mqttserver[64] = "192.168.178.24";
  } network;
  struct {
    uint8_t brightness = 255;
    // Energy colors
    uint8_t red_energy = 118;
    uint8_t green_energy = 62;
    uint8_t blue_energy = 0;
    // Bias colors
    uint8_t red_bias = 51;
    uint8_t green_bias = 0;
    uint8_t blue_bias = 0;
    // Background colors
    uint8_t red_bg = 0;
    uint8_t green_bg = 0;
    uint8_t blue_bg = 0;
    // Flame size
    uint8_t flame_min = 142;
    uint8_t flame_max = 203;
    // Spark size
    uint8_t spark_min = 200;
    uint8_t spark_max = 255;
    // random sparks at second row
    uint8_t random_spark_probability = 2;
    // how much energy is transferred up for a spark per cycle
    uint8_t spark_tfr = 36;
    // spark cells: how much energy is retained from previous cycle
    uint16_t spark_cap = 176;
    // up radiation
    uint16_t up_rad = 19;
    // sidewards radiation
    uint16_t side_rad = 54;
    // passive cells: how much energy is retained from previous cycle
    uint16_t heat_cap = 69;
  } fire;
  struct {
    float timer = 10.0f;
    uint8_t brightness = 70;
    // use dynamic speeds and scale from a 1d noise map or use preset speeds
    bool dynamic_noise = true;
    // scale_p represents the distance between each pixel in the noise map
    float scale_p = 0.15f;
    // speed is how fast the movement is over the axis in the noise map
    float speed_x = 0.1f;
    float speed_y = 0.2f;
    float speed_z = 0.3f;
    // speed_offset_speed is the travel distance through a 1d noise map
    float speed_offset_speed = 0.02f;
    // The speed for changing the hue values
    float speed_hue = 0;
  } noise;
  struct {
    float timer = 10.0f;
    uint8_t brightness = 100;
    uint8_t pixel_density = 64;
    uint8_t fade_in_speed = 8;
    uint8_t fade_out_speed = 4;
  } twinkels;

  // Open the file, but logs errors if something is wrong
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

  // Read config.json from file system
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

  void slider(JsonObject& node, const char* id, const char* name, float value,
              float min = 0, float max = 255, float step = 1) {
    JsonObject leaf = node.createNestedObject(id);
    leaf["name"] = name;
    leaf["type"] = "slider";
    leaf["value"] = value;
    leaf["min"] = min;
    leaf["max"] = max;
    leaf["step"] = step;
  }

  void checkbox(JsonObject& node, const char* id, const char* name,
                boolean value) {
    JsonObject leaf = node.createNestedObject(id);
    leaf["name"] = name;
    leaf["type"] = "checkbox";
    leaf["value"] = value;
  }

  void text(JsonObject& node, const char* id, const char* name,
            const char* value, uint8_t size) {
    JsonObject leaf = node.createNestedObject(id);
    leaf["name"] = name;
    leaf["type"] = "text";
    leaf["value"] = value;
    leaf["size"] = size;
  }

  // Serializes configuration and creates gui.json in buffer string
  void serialize(String& buffer) {
    DynamicJsonDocument doc(CONFIG_DOC_SIZE);
    /* -------------------------- SETTINGS -----------------------------------*/
    JsonObject settings = doc.createNestedObject("settings");
    settings["name"] = "Configuration Settings";
    /* ------------------------------ DISPLAY --------------------------------*/
    JsonObject settings_display = settings.createNestedObject("display");
    settings_display["name"] = "Display Settings";
    slider(settings_display, "max_milliamps", "Max mAmps",
           display.max_milliamps, 0, 10000, 100);
    checkbox(settings_display, "gamma_correct", "Gamma Correction",
             display.gamma_correct);
    slider(settings_display, "active_animation", "Active Animation",
           Animation::get(), 0, 2);
    slider(settings_display, "chart_timer", "Grafiek Update Interval",
           display.chart_timer, 0.01f, 2.0f, 0.01f);
    slider(settings_display, "chart_size", "Grafiek grote", display.chart_size,
           0, 1000, 1);
    /* ------------------------------ NETWORK --------------------------------*/
    JsonObject settings_network = settings.createNestedObject("network");
    settings_network["name"] = "Network Settings";
    text(settings_network, "ssid", "Network SSID", network.ssid, 32);
    text(settings_network, "password", "Password", network.password, 64);
    text(settings_network, "hostname", "Hostname", network.hostname, 64);
    /* -------------------------- ANIMATIONS ---------------------------------*/
    JsonObject animations = doc.createNestedObject("animations");
    animations["name"] = "Animation Settings";
    /* ------------------------------ FIRE
     * ------------------------------------*/
    JsonObject animations_fire = animations.createNestedObject("fire");
    animations_fire["name"] = "Burning Fire";
    animations_fire["index"] = 0;

    slider(animations_fire, "brightness", "Brightness", fire.brightness);
    slider(animations_fire, "red_energy", "Red Energy", fire.red_energy);
    slider(animations_fire, "green_energy", "Green Energy", fire.green_energy);
    slider(animations_fire, "blue_energy", "Blue Energy", fire.blue_energy);
    slider(animations_fire, "red_bias", "Red Bias", fire.red_bias);
    slider(animations_fire, "green_bias", "Green Bias", fire.green_bias);
    slider(animations_fire, "blue_bias", "Blue Bias", fire.blue_bias);
    slider(animations_fire, "red_bg", "Red Background", fire.red_bg);
    slider(animations_fire, "green_bg", "Green Background", fire.green_bg);
    slider(animations_fire, "blue_bg", "Blue Background", fire.blue_bg);

    slider(animations_fire, "random_spark_probability", "Spark Probability",
           fire.random_spark_probability, 0, 100);
    slider(animations_fire, "spark_tfr", "Spark Up Energy", fire.spark_tfr);
    slider(animations_fire, "spark_cap", "Spark Energy Retention",
           fire.spark_cap);
    slider(animations_fire, "flame_min", "Flame Minimal", fire.flame_min);
    slider(animations_fire, "flame_max", "Flame Maximal", fire.flame_max);
    slider(animations_fire, "spark_min", "Spark Minimal", fire.spark_min);
    slider(animations_fire, "spark_max", "Spark Maximal", fire.spark_max);

    slider(animations_fire, "up_rad", "Upwards Radiation", fire.up_rad);
    slider(animations_fire, "side_rad", "Sidewards Radiation", fire.side_rad);
    slider(animations_fire, "heat_cap", "Passive Energy Retention",
           fire.heat_cap);
    /* ------------------------------ NOISE
     * -----------------------------------*/
    JsonObject animations_noise = animations.createNestedObject("noise");
    animations_noise["name"] = "Dynamic Noise";
    animations_noise["index"] = 1;

    slider(animations_noise, "timer", "Palette Interval", noise.timer, 0, 60,
           1);
    slider(animations_noise, "brightness", "Brightness", noise.brightness);
    slider(animations_noise, "speed_hue", "Rotate Hue", noise.speed_hue);

    checkbox(animations_noise, "dynamic_noise", "Dynamic Noise",
             noise.dynamic_noise);
    slider(animations_noise, "speed_offset_speed", "Dynamic Speed",
           noise.speed_offset_speed, 0.00f, 0.10f, 0.001f);
    slider(animations_noise, "scale_p", "Scale", noise.scale_p, 0.0f, 1.0f,
           0.001f);
    slider(animations_noise, "speed_x", "X-Speed", noise.speed_x, -3.0f, 3.0f,
           0.01f);
    slider(animations_noise, "speed_y", "Y-Speed", noise.speed_y, -3.0f, 3.0f,
           0.01f);
    slider(animations_noise, "speed_z", "Z-Speed", noise.speed_z, -3.0f, 3.0f,
           0.01f);
    /* ------------------------------ TWINKELS
     * --------------------------------*/
    JsonObject animations_twinkels = animations.createNestedObject("twinkels");
    animations_twinkels["name"] = "Twinkeling twilight";
    animations_twinkels["index"] = 2;

    slider(animations_twinkels, "timer", "Palette Interval", twinkels.timer, 0,
           60, 1);
    slider(animations_twinkels, "brightness", "Brightness",
           twinkels.brightness);
    slider(animations_twinkels, "pixel_density", "Density",
           twinkels.pixel_density);
    slider(animations_twinkels, "fade_in_speed", "Fade in speed",
           twinkels.fade_in_speed);
    slider(animations_twinkels, "fade_out_speed", "Fade out speed",
           twinkels.fade_out_speed);

    serializeJson(doc, buffer);
  }

  // Deserializes message from gui to config struct
  void deserialize(String buffer) {
    DynamicJsonDocument doc(CONFIG_DOC_SIZE);
    DeserializationError err = deserializeJson(doc, buffer);
    if (err) {
      Serial.printf("Deserialization error: %s\n", err.c_str());
      return;
    }
    /* ------------------------------ DISPLAY --------------------------------*/
    display.max_milliamps =
        doc["settings"]["display"]["max_milliamps"]["value"] |
        display.max_milliamps;
    display.gamma_correct =
        doc["settings"]["display"]["gamma_correct"]["value"] |
        display.gamma_correct;
    Animation::set(doc["settings"]["display"]["active_animation"]["value"] |
                   Animation::get());
    display.chart_timer = doc["settings"]["display"]["chart_timer"]["value"] |
                          display.chart_timer;
    display.chart_size =
        doc["settings"]["display"]["chart_size"]["value"] | display.chart_size;
    /* ------------------------------ NETWORK --------------------------------*/
    strlcpy(network.ssid,
            doc["settings"]["network"]["ssid"]["value"] | network.ssid,
            sizeof(network.ssid));
    strlcpy(network.hostname,
            doc["settings"]["network"]["hostname"]["value"] | network.hostname,
            sizeof(network.hostname));
    strlcpy(network.password,
            doc["settings"]["network"]["password"]["value"] | network.password,
            sizeof(network.password));
    /* ------------------------------ FIRE -----------------------------------*/
    fire.brightness =
        doc["animations"]["fire"]["brightness"]["value"] | fire.brightness;
    fire.red_energy =
        doc["animations"]["fire"]["red_energy"]["value"] | fire.red_energy;
    fire.green_energy =
        doc["animations"]["fire"]["green_energy"]["value"] | fire.green_energy;
    fire.blue_energy =
        doc["animations"]["fire"]["blue_energy"]["value"] | fire.blue_energy;
    fire.red_bias =
        doc["animations"]["fire"]["red_bias"]["value"] | fire.red_bias;
    fire.green_bias =
        doc["animations"]["fire"]["green_bias"]["value"] | fire.green_bias;
    fire.blue_bias =
        doc["animations"]["fire"]["blue_bias"]["value"] | fire.blue_bias;
    fire.red_bg = doc["animations"]["fire"]["red_bg"]["value"] | fire.red_bg;
    fire.green_bg =
        doc["animations"]["fire"]["green_bg"]["value"] | fire.green_bg;
    fire.blue_bg = doc["animations"]["fire"]["blue_bg"]["value"] | fire.blue_bg;

    fire.random_spark_probability =
        doc["animations"]["fire"]["random_spark_probability"]["value"] |
        fire.random_spark_probability;
    fire.spark_tfr =
        doc["animations"]["fire"]["spark_tfr"]["value"] | fire.spark_tfr;
    fire.spark_cap =
        doc["animations"]["fire"]["spark_cap"]["value"] | fire.spark_cap;
    fire.flame_min =
        doc["animations"]["fire"]["flame_min"]["value"] | fire.flame_min;
    fire.flame_max =
        doc["animations"]["fire"]["flame_max"]["value"] | fire.flame_max;
    fire.spark_min =
        doc["animations"]["fire"]["spark_min"]["value"] | fire.spark_min;
    fire.spark_max =
        doc["animations"]["fire"]["spark_max"]["value"] | fire.spark_max;

    fire.up_rad = doc["animations"]["fire"]["up_rad"]["value"] | fire.up_rad;
    fire.side_rad =
        doc["animations"]["fire"]["side_rad"]["value"] | fire.side_rad;
    fire.heat_cap =
        doc["animations"]["fire"]["heat_cap"]["value"] | fire.heat_cap;
    /* ------------------------------ NOISE ----------------------------------*/
    noise.timer = doc["animations"]["noise"]["timer"]["value"] | noise.timer;
    noise.brightness =
        doc["animations"]["noise"]["brightness"]["value"] | noise.brightness;
    noise.dynamic_noise = doc["animations"]["noise"]["dynamic_noise"]["value"] |
                          noise.dynamic_noise;
    noise.scale_p =
        doc["animations"]["noise"]["scale_p"]["value"] | noise.scale_p;
    noise.speed_x =
        doc["animations"]["noise"]["speed_x"]["value"] | noise.speed_x;
    noise.speed_y =
        doc["animations"]["noise"]["speed_y"]["value"] | noise.speed_y;
    noise.speed_z =
        doc["animations"]["noise"]["speed_z"]["value"] | noise.speed_z;
    noise.speed_offset_speed =
        doc["animations"]["noise"]["speed_offset_speed"]["value"] |
        noise.speed_offset_speed;
    noise.speed_hue =
        doc["animations"]["noise"]["speed_hue"]["value"] | noise.speed_hue;
    /* ------------------------------ TWINKELS
     * --------------------------------*/
    twinkels.timer =
        doc["animations"]["twinkels"]["timer"]["value"] | twinkels.timer;
    twinkels.brightness = doc["animations"]["twinkels"]["brightness"]["value"] |
                          twinkels.brightness;
    twinkels.fade_in_speed =
        doc["animations"]["twinkels"]["fade_in_speed"]["value"] |
        twinkels.fade_in_speed;
    twinkels.fade_out_speed =
        doc["animations"]["twinkels"]["fade_out_speed"]["value"] |
        twinkels.fade_out_speed;
    twinkels.pixel_density =
        doc["animations"]["twinkels"]["pixel_density"]["value"] |
        twinkels.pixel_density;
  }

  // Executes json command send from the client
  void execute(uint8_t* char_buffer) {
    StaticJsonDocument<COMMAND_DOC_SIZE> doc;
    DeserializationError err = deserializeJson(doc, char_buffer);
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
      String string_buffer;
      // event is not needed anymore, remove from document
      doc.remove("event");
      // serialize the document to a string buffer
      serializeJson(doc, string_buffer);
      // deserialize string buffer to the config struct
      deserialize(string_buffer);
    } else if (event.equals("save")) {
      save();
    }
  }
};
// All cpp files that include this link to a single config struct
extern struct Config config;
#endif