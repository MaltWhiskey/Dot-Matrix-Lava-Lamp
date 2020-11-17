#ifndef NOISE_H
#define NOISE_H
#include <stdint.h>
#include "Animation.h"
#include "NoiseGenerator.h"
#include "Timer.h"
#include "Palettes.h"
#include "Display.h"
/*---------------------------------------------------------------------------------------
 * Noise ?
 *-------------------------------------------------------------------------------------*/
class Noise : public Animation {
 private:
  Timer timer;
  NoiseGenerator ng;
  Palettes palettes;

  // start somewhere in the noise map
  float noise_x = ng.nextRandom(0, 255);
  float noise_y = ng.nextRandom(0, 255);
  float noise_z = ng.nextRandom(0, 255);
  // Allocate noise memory
  uint8_t noise[display.width][display.height];

  // Adds a rotating value to hue when getting colors from palettes
  float hue = 0;
  // speed_offset is used to travel a 1d noise map and get the axis speeds
  float speed_offset = 0;
  // scale_p represents the distance between each pixel in the noise map
  float scale_p = 0.15f;
  // speed is how fast the movement is over the axis in the noise map
  float speed_x = 0.1f;
  float speed_y = 0.2f;
  float speed_z = 0.3f;

  // Current palette continually blends to target palette
  CRGBPalette16 currentPalette = CRGBPalette16(CRGB::Black);
  CRGBPalette16 targetPalette = CRGBPalette16(CRGB::Black);

 private:
  void init();
  bool draw(float dt);
  void fadeIn() { Display::fadeIn(config.noise.brightness); }
  void fadeOut() { Display::fadeOut(config.noise.brightness); }

  void makeNoise();
  void drawNoise();
  void updateNoise(float dt);
};
#endif