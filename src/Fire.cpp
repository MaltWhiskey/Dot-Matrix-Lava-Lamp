#include "main.h"
#include "Fire.h"
/*---------------------------------------------------------------------------------------
 * FIRE
 *-------------------------------------------------------------------------------------*/
// initialization code for this animation
void Fire::init() {}

bool Fire::draw(float dt) {
  // always set brightness for each animation in draw
  FastLED.setBrightness(config.fire.brightness);
  // generate fire energy
  generateCurrentEnergy();
  // calculate new energy;
  generateNextEnergy();
  // generate colors for displaying
  generateColors();
  return false;
}

void Fire::generateCurrentEnergy() {
  for (int x = 0; x < display.width; x++) {
    // random flame energy at bottom row
    currentEnergy[x][0] = random8(config.fire.flame_min, config.fire.flame_max);
    energyMode[x][0] = torch_nop;
    // random sparks at second row
    if (energyMode[x][1] != torch_spark &&
        random8(0, 100) < config.fire.random_spark_probability) {
      currentEnergy[x][1] = random8(config.fire.spark_min, config.fire.spark_max);
      energyMode[x][1] = torch_spark;
    }
  }
}

void Fire::generateNextEnergy() {
  for (uint8_t y = 0; y < display.height; y++) {
    for (uint8_t x = 0; x < display.width; x++) {
      // Energy from this level
      uint8_t e0 = currentEnergy[x][y];
      switch (energyMode[x][y]) {
        case torch_spark: {
          // loose transfer up energy as long as there is any
          e0 = qsub8(e0, config.fire.spark_tfr);
          // cell above is temp spark, sucking up energy from this cell until
          // empty
          if (y < display.height - 1) energyMode[x][y + 1] = torch_spark_temp;
          break;
        }
        case torch_spark_temp: {
          // Energy from below
          uint8_t e1 = currentEnergy[x][y - 1];
          if (e1 < config.fire.spark_tfr) {
            // cell below is exhausted, becomes passive
            energyMode[x][y - 1] = torch_passive;
            // gobble up rest of energy
            e0 = qadd8(e0, e1);
            // loose some overall energy
            e0 = (e0 * config.fire.spark_cap) >> 8;
            // this cell becomes active spark
            energyMode[x][y] = torch_spark;
          } else {
            e0 = qadd8(e0, config.fire.spark_tfr);
          }
          break;
        }
        case torch_passive: {
          // Energy from below
          uint8_t e1 = currentEnergy[x][y - 1];
          // add energy from previous cycle
          e0 = (e0 * config.fire.heat_cap) >> 8;
          // add energy from lower level
          e0 = qadd8(e0, (e1 * config.fire.up_rad) >> 8);
          // add energy from the sides
          e0 = qadd8(
              e0, ((currentEnergy[(x - 1) & 0xf][y] + currentEnergy[(x + 1) & 0xf][y]) *
                   config.fire.side_rad) >>
                      9);
        }
        case torch_nop:
          break;
      }
      nextEnergy[x][y] = e0;
    }
  }
}

void Fire::generateColors() {
  for (uint8_t y = 0; y < display.height; y++) {
    for (uint8_t x = 0; x < display.width; x++) {
      uint8_t e0 = nextEnergy[x][y];
      currentEnergy[x][y] = e0;
      if (e0 > 0) {
        // energy value to brightness is non-linear
        uint8_t v = energymap[e0 >> 3];
        uint8_t r = config.fire.red_bias;
        uint8_t g = config.fire.green_bias;
        uint8_t b = config.fire.blue_bias;
        r = qadd8(r, (v * config.fire.red_energy) >> 8);
        g = qadd8(g, (v * config.fire.green_energy) >> 8);
        b = qadd8(b, (v * config.fire.blue_energy) >> 8);
        display.leds[x][y] = CRGB(r, g, b);
      } else {
        display.leds[x][y] =
            CRGB(config.fire.red_bg, config.fire.green_bg, config.fire.blue_bg);
      }
    }
  }
}