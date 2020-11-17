#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H
#include <stdint.h>
#include "driver/gpio.h"
#include <driver/dac.h>
#include <driver/adc.h>

class TemperatureSensor {
 private:
  uint32_t voltage2analog[0x101];
  uint32_t analog2voltage[0x1000];

  // Power pins used to power the NTC
  uint8_t gnd_pin;
  uint8_t vcc_pin;
  // ADC Pin used for reading NTC
  uint8_t ntc_pin;
  // DAC and ADC Pin used for calibration
  uint8_t adc_pin;
  dac_channel_t dac_chn;

 private:
  uint16_t running_cnt = 0;
  uint32_t running_avg = 0;

 public:
  TemperatureSensor(uint8_t NTC_PIN, uint8_t GND_PIN, uint8_t VCC_PIN, uint8_t ADC_PIN,
                    dac_channel_t DAC_PIN);
  void calibrate();
  float value();
  float celcius();
};
#endif