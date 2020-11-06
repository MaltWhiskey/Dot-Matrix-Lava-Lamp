#ifndef ANIMATION_H
#define ANIMATION_H
#include <stdint.h>
#include "Timer.h"
#include "Display.h"
/*---------------------------------------------------------------------------------------
 * ANIMATION INTERFACE
 *-------------------------------------------------------------------------------------*/
class Animation {
 protected:
  // Reference to the display (initialized only once)
  static Display& display;

 private:
  // Timer used to call animations making rendering frequency independent
  static Timer timer;
  // Total animation pointers in animation list
  static uint8_t m_iAnimations;
  // Pointers to animations in an animation list
  static Animation* m_pAnimations[20];
  // Current animation in the animation list
  static uint8_t m_iCurrentAnimation;
  // Requested animation in the animation list
  static uint8_t m_iRequestedAnimation;

 public:
  // Constructor inserts this animation in the list
  Animation();
  // virtual destructor
  virtual ~Animation(){};
  // draw method needs to be overridden
  virtual bool draw(float dt) = 0;
  // init method needs to be overridden
  virtual void init() = 0;
  // Fade methods can be overridden
  virtual void fadeIn(){};
  virtual void fadeOut(){};
  // press of the control button
  static void singleClick();
  static void doubleClick();
  // animation scheduler
  static void animate();
  // initialize animation list, first animation and display
  static void begin();

  // Cycle through the animations (not thread safe)
  static void next();
  static void previous();
  static void set(uint8_t index);
  static uint8_t get();
};
#endif