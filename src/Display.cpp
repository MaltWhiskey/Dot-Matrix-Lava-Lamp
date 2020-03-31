#include "main.h"
#include "Display.h"
#include <Arduino.h>
/*----------------------------------------------------------------------------------------------
 * DISPLAY CLASS
 *--------------------------------------------------------------------------------------------*/
void Display::begin() {
  // Setup up alternative power pin for sensor
  pinMode(SWITCH_VCC, OUTPUT);
  pinMode(SWITCH_GND, OUTPUT);
  digitalWrite(SWITCH_VCC, HIGH);
  digitalWrite(SWITCH_GND, LOW);
  // Setup analog and digital inputs
  pinMode(SWITCH_PIN, INPUT);
  // Setup led controller and output pin
  FastLED.addLeds<WS2811, LEDPIN, GRB>(leds_, width * height);
  FastLED.setDither(false);
}

void Display::update() {
  // arrange display (flip upside down and zig zag)
  for (uint8_t y = 0; y < height; y += 2) {
    uint8_t i = y << 4;
    for (uint8_t x = 0; x < width; x++) {
      if (config.display.gamma_correct) {
        leds[x][y].r = gamma8[leds[x][y].r];
        leds[x][y].g = gamma8[leds[x][y].g];
        leds[x][y].b = gamma8[leds[x][y].b];
        leds[x][y + 1].r = gamma8[leds[x][y + 1].r];
        leds[x][y + 1].g = gamma8[leds[x][y + 1].g];
        leds[x][y + 1].b = gamma8[leds[x][y + 1].b];
      }
      leds_[255 - (i + x)] = leds[x][y];
      leds_[255 - (i - x + width + width - 1)] = leds[x][y + 1];
    }
  }
  // Maximize power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5, config.display.max_milliamps);
  // Update led display
  FastLED.show();
}

void Display::fadeIn(uint8_t &brightness) {
  uint16_t b16 = brightness * 2;
  if (b16 == 0)
    brightness = 1;
  else if (b16 > 255)
    brightness = 255;
  else
    brightness = b16;
}
void Display::fadeOut(uint8_t &brightness) {
  // fade out step smaller than fadeInt for fine tuning
  brightness *= 0.8;
}

const uint8_t Display::gamma8[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,
    2,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   5,   5,   5,
    5,   6,   6,   6,   6,   7,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,
    10,  10,  11,  11,  11,  12,  12,  13,  13,  13,  14,  14,  15,  15,  16,  16,
    17,  17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  24,  24,  25,
    25,  26,  27,  27,  28,  29,  29,  30,  31,  32,  32,  33,  34,  35,  35,  36,
    37,  38,  39,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  50,
    51,  52,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  66,  67,  68,
    69,  70,  72,  73,  74,  75,  77,  78,  79,  81,  82,  83,  85,  86,  87,  89,
    90,  92,  93,  95,  96,  98,  99,  101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};