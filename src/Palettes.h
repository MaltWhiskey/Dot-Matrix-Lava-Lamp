#ifndef PALETTES_H
#define PALETTES_H
#include <stdint.h>
#include <FastLED.h>
/*----------------------------------------------------------------------------------------------
 * Palettes CLASS
 *--------------------------------------------------------------------------------------------*/
class Palettes {
 private:
  uint8_t m_noise_palette_index = 0;
  uint8_t m_twinkel_palette_index = 0;
  uint8_t m_noise_palette_count = 0;
  uint8_t m_twinkel_palette_count = 0;

 public:
  Palettes();
  CRGBPalette16 getNoisePalette();
  CRGBPalette16 getTwinkelPalette();
};
#endif