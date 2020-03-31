#ifndef MAIN_H
#define MAIN_H
#include <stdint.h>
/*---------------------------------------------------------------------------------------
 * Evil global Config parameters
 *
 * Animation init or draw routines need to apply config parameters to dynamically set
 * runtime parameters. Init only gets called when an animation starts or restarts
 * draw gets called every animation frame so choose wisely where to apply.
 *
 * TODO: make this available to change via web interface and save to persistent memory
 *-------------------------------------------------------------------------------------*/
struct Config {
  struct {
    uint16_t max_milliamps = 8000;
    bool gamma_correct = false;
  } display;
  struct {
    const char ssid[32] = "-^..^-";
    const char password[64] = "qazwsxEDC1";
    const char hostname[64] = "esp32";
  } network;
  struct {
    float timer = 10.0f;
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
};
extern struct Config config;
#endif