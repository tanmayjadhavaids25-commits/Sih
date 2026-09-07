#ifndef IDR_DISPLAY_H
#define IDR_DISPLAY_H

#include <Arduino.h>
#include <MCUFRIEND_kbv.h>
#include "navigation.h"

struct DisplayContext {
  MCUFRIEND_kbv tft;
  uint16_t tftId;
  int16_t width;
  int16_t height;
  DisplayPage page;
  bool staticDrawn;
  NavigationState lastState;
  bool hasLastState;
  int16_t lastVehicleX;
  int16_t lastVehicleY;
  uint8_t lastVehicleDir;
  unsigned long blinkMs;
  bool blinkOn;
};

void initDisplay(DisplayContext &display);
void showBootScreen(DisplayContext &display);
void setDisplayPage(DisplayContext &display, DisplayPage page);
void drawCurrentPage(DisplayContext &display, const NavigationState &state, bool forceStatic);

void drawNavigationMap(DisplayContext &display, const NavigationState &state, bool fullRedraw);
void drawVehicleMarker(DisplayContext &display, int16_t x, int16_t y, float heading, uint16_t color);
void updateVehiclePosition(DisplayContext &display, const NavigationState &state);
void drawHeadingArrow(DisplayContext &display, float heading);
void drawSpeed(DisplayContext &display, float speed);
void drawHeading(DisplayContext &display, float heading);
void drawConfidence(DisplayContext &display, float confidence);

#endif
