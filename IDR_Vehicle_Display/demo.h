#ifndef IDR_DEMO_H
#define IDR_DEMO_H

#include <Arduino.h>
#include "navigation.h"

struct DemoController {
  bool running;
  bool paused;
  uint8_t stage;
  unsigned long stageStartMs;
  unsigned long lastMoveMs;
  uint16_t routeIndex;
};

void initDemoController(DemoController &demo);
void startDemo(DemoController &demo, NavigationState &state);
void pauseDemo(DemoController &demo);
void resetDemo(DemoController &demo, NavigationState &state);
void updateMockNavigation(DemoController &demo, NavigationState &state, unsigned long nowMs);

#endif
