#include "main.h"
#include "Noise.h"
/*---------------------------------------------------------------------------------------
 * Noise
 *-------------------------------------------------------------------------------------*/
// initialization code for this animation
void Noise::init() {
  // timer for X seconds, on expiration init() is called
  timer = config.noise.timer;
  // each time init is called cycle to the next palette
  targetPalette = palettes.getNoisePalette();
}

bool Noise::draw(float dt) {
  // always set brightness for each animation in draw
  FastLED.setBrightness(config.noise.brightness);
  updateNoise(dt);
  makeNoise();
  drawNoise();
  // returning true lets the animation::animate() call init() again
  return (timer.update());
}

// Create the 2D noise data matrix[][]
void Noise::makeNoise() {
  for (int x = 0; x < display.width; x++) {
    float xoffset = noise_x + scale_p * x;
    for (int y = 0; y < display.height; y++) {
      float yoffset = noise_y + scale_p * y;
      noise[x][y] = ng.noise3(xoffset, yoffset, noise_z) * 255;
    }
  }
}

// Draw the 2D noise data matrix[][]
void Noise::drawNoise() {
  for (int x = 0; x < display.width; x++) {
    for (int y = 0; y < display.height; y++) {
      // The index at (x,y) is the index in the color palette
      uint8_t index = noise[x][y];
      // The value at (y,x) is the overlay for the brightness
      uint8_t value = noise[y][x];
      // brighten up the overlay map
      if (value > 127)
        value = 255;
      else
        value = dim8_raw(value * 2);
      display.leds[x][y] = ColorFromPalette(currentPalette, index + hue, value);
    }
  }
}

void Noise::updateNoise(float dt) {
  if (config.noise.dynamic_noise) {
    speed_offset += dt * config.noise.speed_offset_speed;
    // use same speed offset, but offset each in the noise map
    speed_x = 2 * (ng.noise1(speed_offset + 000) - 0.5);    //  -1 to 1
    speed_y = 2 * (ng.noise1(speed_offset + 064) - 0.5);    //  -1 to 1
    speed_z = 2 * (ng.noise1(speed_offset + 128) - 0.5);    //  -1 to 1
    scale_p = .15 + (ng.noise1(speed_offset + 196) / 6.6);  // .15 to .30
  } else {
    scale_p = config.noise.scale_p;
    speed_x = config.noise.speed_x;
    speed_y = config.noise.speed_y;
    speed_z = config.noise.speed_z;
  }
  if (config.noise.speed_hue > 0)
    hue += config.noise.speed_hue * dt;
  else
    hue = 0;

  noise_x += config.noise.speed_x * dt;
  noise_y += config.noise.speed_y * dt;
  noise_z += config.noise.speed_z * dt;

  // dt dependent blend towards target palette
  nblendPaletteTowardPalette(currentPalette, targetPalette, 512 * dt);
}