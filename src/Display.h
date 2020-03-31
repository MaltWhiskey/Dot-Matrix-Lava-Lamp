#ifndef DISPLAY_H
#define DISPLAY_H
#define FASTLED_ESP32_I2S
#define FASTLED_INTERNAL
#include <stdint.h>
#include <FastLED.h>
/*---------------------------------------------------------------------------------------
 * Matrix 16 x 16
 *
 * The 1st led starts at bottom row to the left, than up one row and to the
 * right, repeat
 *
 * - FF--------------F0 <
 * > E0--------------EF ^
 *
 * ^ 3F--------------30 <
 * > 20--------------2F ^
 * ^ 1F--------------10 <
 * > 00--------------0F ^
 * --------------------------------------------------------------------------------------*/
#define LEDPIN 16
#define SWITCH_VCC 17
#define SWITCH_PIN 21
#define SWITCH_GND 22

class Display {
 public:
  static const uint8_t width = 16;
  static const uint8_t height = 16;
  CRGB leds[width][height];
  static const uint8_t gamma8[256];

 public:
  // Gets a display reference to enable calling public methods
  static Display &instance() {
    // Create exactly one Display object with the private contructor
    static Display display;
    // Return the one and only display object
    return display;
  };
  void begin();
  void update();
  // Fading functions updates the config brightness
  static void fadeIn(uint8_t &brightness);
  static void fadeOut(uint8_t &brightness);

 private:
  CRGB leds_[width * height];
  // Not allowed to make an object of the class, implemented singleton pattern
  Display(){};
  // Copy constructor is also disabled
  Display(Display const &);
  // Assigment operator is also disabled
  void operator=(Display const &);
};
#endif