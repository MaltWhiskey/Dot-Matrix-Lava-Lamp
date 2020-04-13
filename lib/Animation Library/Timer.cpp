#include "Timer.h"
/*----------------------------------------------------------------------------------------------
 * TIMER CLASS
 *--------------------------------------------------------------------------------------------*/
Timer::Timer() {}
Timer::Timer(const float alarm) { operator=(alarm); }
void Timer::operator=(const float alarm) {
  m_alarmCount = 0;
  m_startTime = 0;
  m_alarmTime = alarm;
}
uint16_t Timer::update() {
  m_currentTime = micros();
  if (m_startTime == 0) {
    m_startTime = m_currentTime;
    m_lastTime = m_currentTime;
  }
  m_deltaTime = m_currentTime - m_lastTime;
  m_runTime = m_currentTime - m_startTime;
  m_lastTime = m_currentTime;
  if (m_alarmTime > 0) {
    unsigned int threshold = m_runTime / (m_alarmTime * 1000000.0f);
    if (m_alarmCount < threshold) {
      m_alarmCount = threshold;
      return m_alarmCount;
    }
  }
  return 0;
}
float Timer::dt() const { return m_deltaTime / 1000000.0f; }
float Timer::rt() const { return m_runTime / 1000000.0f; }