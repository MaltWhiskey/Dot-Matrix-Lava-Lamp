#ifndef TWINKELS_H
#define TWINKELS_H
#include <stdint.h>
#include <FastLED.h>
#include "Animation.h"
#include "Palettes.h"
/*---------------------------------------------------------------------------------------
 * Twinkels
 *-------------------------------------------------------------------------------------*/
class Twinkels : public Animation {
 private:
  Timer timer;
  Palettes palettes;

  uint8_t pixel_density = 64;
  uint8_t fade_in_speed = 8;
  uint8_t fade_out_speed = 4;

  // keep source color for the display to prevent scaling errors
  CRGB colors[display.width][display.height];
  // amount the source is scaled 0 to 255
  uint8_t scaling[display.width][display.height] = {};
  // the source is scaled from black to color or color to black
  bool brighten[display.width][display.height] = {};
  CRGBPalette16 currentPalette = CRGBPalette16(CRGB::Black);

 private:
  void init();
  bool draw(float dt);
  void fadeIn() { Display::fadeIn(config.twinkels.brightness); }
  void fadeOut() { Display::fadeOut(config.twinkels.brightness); }
};
#endif