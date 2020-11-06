#include "Animation.h"
/*---------------------------------------------------------------------------------------
 * ANIMATION STATIC DEFINITIONS
 *-------------------------------------------------------------------------------------*/
// Reference to the display (initialized only once)
Display &Animation::display = Display::instance();
// Timer used to call animations making rendering frequency independent
Timer Animation::timer = Timer();
// Total animation pointers in animation list
uint8_t Animation::m_iAnimations = 0;
// Current animation in the animation list
uint8_t Animation::m_iCurrentAnimation = 0;
// Requested animation in the animation list
uint8_t Animation::m_iRequestedAnimation = 0;
// Pointers to animations in a animation list
Animation *Animation::m_pAnimations[20];

Animation::Animation() {
  // Add this animation to animation list
  m_pAnimations[m_iAnimations++] = this;
}

void Animation::next() {
  // Using the temp variable makes this sort of thread safe...
  uint8_t requested = m_iCurrentAnimation + 1;
  if (requested >= m_iAnimations) requested = 0;
  m_iRequestedAnimation = requested;
}

void Animation::previous() {
  // Using the temp variable makes this sort of thread safe...
  uint8_t requested = m_iCurrentAnimation - 1;
  if (requested == 0xff) requested = m_iAnimations - 1;
  m_iRequestedAnimation = requested;
}

void Animation::set(uint8_t requested) {
  // Using the temp variable makes this sort of thread safe...
  uint8_t current = m_iCurrentAnimation;
  if (requested != current && requested < m_iAnimations) {
    m_iRequestedAnimation = requested;
  }
}

uint8_t Animation::get() { return m_iCurrentAnimation; }

void Animation::singleClick() {
  // Using the temp variable makes this sort of thread safe...
  uint8_t current = m_iCurrentAnimation;
  if (m_iAnimations > 0 && current < m_iAnimations) {
    m_pAnimations[current]->fadeIn();
  }
}

void Animation::doubleClick() {
  // Using the temp variable makes this sort of thread safe...
  uint8_t current = m_iCurrentAnimation;
  if (m_iAnimations > 0 && current < m_iAnimations) {
    m_pAnimations[current]->fadeOut();
  }
}

void Animation::animate() {
  // Using the temp variable makes this sort of thread safe...
  uint8_t current = m_iCurrentAnimation;
  uint8_t requested = m_iRequestedAnimation;
  // update the animation timer and get deltatime between previous frame
  timer.update();
  // if there are animations present and no out of bounds on current animation
  if (m_iAnimations > 0 && current < m_iAnimations) {
    // draw one animation frame using the deltatime and the display dimensions
    if (m_pAnimations[current]->draw(timer.dt())) {
      m_pAnimations[current]->init();
    }
    // Check if new animation needs to be initialized and displayed
    if (current != requested && requested < m_iAnimations) {
      m_pAnimations[requested]->init();
      m_iCurrentAnimation = requested;
    }
    display.update();
  }
}

void Animation::begin() {
  // if there are animations present
  if (m_iAnimations > 0) {
    m_pAnimations[m_iCurrentAnimation]->init();
  }
  // Initialize the display
  display.begin();
}