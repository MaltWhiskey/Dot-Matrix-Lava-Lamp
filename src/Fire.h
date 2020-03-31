#ifndef FIRE_H
#define FIRE_H
#include <stdint.h>
#include "Animation.h"
/*---------------------------------------------------------------------------------------
 * FIRE
 *-------------------------------------------------------------------------------------*/
class Fire : public Animation {
 private:
  const uint8_t energymap[32] = {0,   64,  96,  112, 128, 144, 152, 160, 168, 176, 184,
                                 184, 192, 200, 200, 208, 208, 216, 216, 224, 224, 224,
                                 232, 232, 232, 240, 240, 240, 240, 248, 248, 248};

  uint8_t flame_min = 100;
  uint8_t flame_max = 200;

  uint8_t random_spark_probability = 2;
  uint8_t spark_min = 200;
  uint8_t spark_max = 255;

  // how much energy is transferred up for a spark per cycle
  uint8_t spark_tfr = 40;
  // spark cells: how much energy is retained from previous cycle
  uint16_t spark_cap = 200;
  // up radiation
  uint16_t up_rad = 35;
  // sidewards radiation
  uint16_t side_rad = 35;
  // passive cells: how much energy is retained from previous cycle
  uint16_t heat_cap = 20;

  uint8_t red_bg = 0;
  uint8_t green_bg = 0;
  uint8_t blue_bg = 0;

  uint8_t red_bias = 10;
  uint8_t green_bias = 0;
  uint8_t blue_bias = 0;

  uint8_t red_energy = 180;
  uint8_t green_energy = 30;
  uint8_t blue_energy = 0;

  uint8_t currentEnergy[display.width][display.height];
  uint8_t nextEnergy[display.width][display.height];
  uint8_t energyMode[display.width][display.height];

  enum {
    // just environment, glow from nearby radiation
    torch_passive = 0,
    // slowly looses energy, moves up
    torch_spark = 1,
    // a spark still getting energy from the level below
    torch_spark_temp = 2,
    // no processing
    torch_nop = 4
  };

  Timer timer;

 private:
  void init();
  bool draw(float dt);
  void fadeIn() { Display::fadeIn(config.fire.brightness); }
  void fadeOut() { Display::fadeOut(config.fire.brightness); }

  void generateCurrentEnergy();
  void generateNextEnergy();
  void generateColors();
};
#endif