#include "main.h"
#include "Twinkels.h"
/*---------------------------------------------------------------------------------------
 * Twinkels
 *-------------------------------------------------------------------------------------*/
// initialization code for this animation
void Twinkels::init() {
  // timer for X seconds, on expiration init() is called
  timer = config.twinkels.timer;
  // each time init is called cycle to the next palette
  currentPalette = palettes.getTwinkelPalette();
}

bool Twinkels::draw(float dt) {
  // always set brightness for each animation in draw
  FastLED.setBrightness(config.twinkels.brightness);
  // set all other config dependent parameters
  pixel_density = config.twinkels.pixel_density;
  fade_in_speed = config.twinkels.fade_in_speed;
  fade_out_speed = config.twinkels.fade_out_speed;

  for (uint8_t x = 0; x < display.width; x++) {
    for (uint8_t y = 0; y < display.height; y++) {
      if (brighten[x][y]) {
        scaling[x][y] = qadd8(scaling[x][y], fade_in_speed);
        // if target color is reached reverse direction
        if (scaling[x][y] == 255) brighten[x][y] = false;
      } else {
        scaling[x][y] = qsub8(scaling[x][y], fade_out_speed);
      }
      display.leds[x][y] = colors[x][y];
      display.leds[x][y].nscale8(scaling[x][y]);
    }
  }

  // Consider adding a new random twinkle
  if (random8() < pixel_density) {
    uint8_t x = random16(display.width);
    uint8_t y = random16(display.height);
    if (scaling[x][y] == 0) {
      colors[x][y] = ColorFromPalette(currentPalette, random8(), 255);
      brighten[x][y] = true;
    }
  }
  // returning true lets the animation::animate() call init() again
  return (timer.update());
}