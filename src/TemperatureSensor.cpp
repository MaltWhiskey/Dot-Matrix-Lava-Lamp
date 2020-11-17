#include <Arduino.h>
#include "TemperatureSensor.h"
/*----------------------------------------------------------------------------------------------
 * TEMPERATURE SENSOR CLASS
 *
 * Connect GPIO 25 to GPIO 35 to allow calibrating the ADC by using the internal DAC
 *--------------------------------------------------------------------------------------------*/
TemperatureSensor::TemperatureSensor(uint8_t NTC_PIN, uint8_t GND_PIN, uint8_t VCC_PIN,
                                     uint8_t ADC_PIN, dac_channel_t DAC_CHN) {
  ntc_pin = NTC_PIN;
  gnd_pin = GND_PIN;
  vcc_pin = VCC_PIN;
  adc_pin = ADC_PIN;
  dac_chn = DAC_CHN;
}

void TemperatureSensor::calibrate() {
  Serial.printf("Calibrating temperature sensor\n");
  // Setup up power pin for NTC
  pinMode(gnd_pin, OUTPUT);
  digitalWrite(gnd_pin, LOW);
  pinMode(vcc_pin, OUTPUT);
  digitalWrite(vcc_pin, HIGH);
  pinMode(ntc_pin, INPUT);

  Serial.printf("Configure ADC\n");
  // Configure ADC1 ONLY!!! (ADC2 is used by WiFi)
  analogReadResolution(12);
  Serial.printf("Enable DAC Channel\n");
  dac_output_enable(dac_chn);

  Serial.printf("Calibrating ADC\n");
  // set the dac voltage 256 times from 0 to 255
  for (uint16_t v = 0; v <= 0xff; v++) {
    dac_output_voltage(dac_chn, v);
    uint32_t value = 0;
    // sample 256 times the adc and add together (12bit * 256 -> 20bit)
    for (uint32_t i = 0; i <= 0xff; i++) {
      value += analogRead(adc_pin);
    }
    voltage2analog[v] = value;
  }
  dac_output_voltage(dac_chn, 0);
  voltage2analog[0x100] = voltage2analog[0xff];

  for (uint16_t v = 0; v <= 0xff; v++) {
    float delta = (voltage2analog[v + 1] - voltage2analog[v]) / 0x10;
    for (uint16_t w = 0; w <= 0xf; w++) {
      analog2voltage[(v << 4) + w] = voltage2analog[v] + w * delta;
    }
  }
  Serial.printf("Calibrated ADC. Values interpolated from DAC\n");
  Serial.printf("12 bit index interpolated to 18 bits with oversampling\n");
  Serial.printf("Excerpt:\n");
  for (uint16_t v = 0; v <= 0xfff; v += 0x100) {
    Serial.printf("%d -> %d\n", v, analog2voltage[v]);
  }
}

float TemperatureSensor::value() {
  // Turn on current to NTC
  digitalWrite(vcc_pin, HIGH);
  // Comvert read 12 bits to 20 bits
  running_avg += analog2voltage[analogRead(ntc_pin)];
  running_cnt++;
  // Turn off current to NTC
  digitalWrite(vcc_pin, LOW);
  // After 100 measurements correct
  if (running_cnt == 100) {
    running_cnt >>= 2;
    running_avg >>= 2;
  }
  // Convert 20 bits back to 12 bits
  return float(running_avg) / (255 * running_cnt);
}

float TemperatureSensor::celcius() {
  double a = 0.00120078369047805450;
  double b = 0.00021766328063486655;
  double c = 1.8913047351323314e-7;

  float V = value();
  float R = 9870 * (4095 / V - 1);
  float L = log(R);
  float T = (1.0 / (a + b * L + c * L * L * L)) - 273.15;
  return T;
}