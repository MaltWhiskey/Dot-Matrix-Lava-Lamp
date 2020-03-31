#include "main.h"
#include "Fire.h"
#include "Noise.h"
#include "Twinkels.h"
#include "WebServer.h"
#include "OneButton.h"
#include "Display.h"
#include <Arduino.h>
/*---------------------------------------------------------------------------------------
 * Globals
 *-------------------------------------------------------------------------------------*/
// Configuration parameters
Config config;
// Web server used for gui and saving config
WebServer server;
// TaskHandles for running tasks on different cores
TaskHandle_t Task1;
TaskHandle_t Task2;
// Forward declare task functions
void task1(void *);
void task2(void *);
// The front button on the Lava Lamp
OneButton button(SWITCH_PIN, false);
/*---------------------------------------------------------------------------------------
 * Global animations. The animation constructor adds these to a linked list
 *-------------------------------------------------------------------------------------*/
Fire fire;
Noise noise;
Twinkels twinkels;
/*---------------------------------------------------------------------------------------
 * Initialize setup parameters
 *-------------------------------------------------------------------------------------*/
void setup() {
  // Enable console logging
  Serial.begin(115200);
  // Initialize animation display
  Animation::begin();
  // Initialize web server communication
  server.begin();
  delay(500);

  // Create task1 on core 0
  xTaskCreatePinnedToCore(task1, "Task1", 10000, NULL, 1, &Task1, 0);
  delay(500);

  // Create task2 on core 1 (higher number is higher priority)
  xTaskCreatePinnedToCore(task2, "Task2", 10000, NULL, 2, &Task2, 1);
  delay(500);

  // Attach button callback event handlers
  button.attachClick(Animation::singleClick);
  button.attachDoubleClick(Animation::doubleClick);
  button.attachLongPressStart(Animation::next);
}
/*---------------------------------------------------------------------------------------
 * Loop Core 1
 *-------------------------------------------------------------------------------------*/
void loop() { yield(); }
/*---------------------------------------------------------------------------------------
 * Task1 Core 0
 *-------------------------------------------------------------------------------------*/
void task1(void *parameter) {
  Serial.print("Task1: Wifi running on core ");
  Serial.println(xPortGetCoreID());
  while (true) {
    // Check for Web server events
    server.update();
    // Prevents watchdog timeout
    vTaskDelay(1);
  }
}
/*---------------------------------------------------------------------------------------
 * Task2 Core 1
 *-------------------------------------------------------------------------------------*/
void task2(void *parameter) {
  Serial.print("Task2: Animation running on core ");
  Serial.println(xPortGetCoreID());
  // Run animation on blocking fastled. Interrupts mess up the timeing routine
  // Need a way to disable interrupts and watchdog on Core 1 or find a nonblocking
  // FastLed controller (DMA based?)
  while (true) {
    Animation::animate();
    // Check for button 'events' and call them
    button.tick();
  }
}