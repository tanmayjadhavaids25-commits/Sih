#include <Arduino.h>

#include "config.h"
#include "demo.h"
#include "display.h"
#include "navigation.h"
#include "serial_protocol.h"

#if ENABLE_TOUCH
#include <TouchScreen.h>
TouchScreen ts(TOUCH_XP, TOUCH_YP, TOUCH_XM, TOUCH_YM, 300);
#endif

DisplayContext gDisplay;
NavigationState gState;
SerialProtocolContext gSerial;
DemoController gDemo;

unsigned long gLastUiUpdateMs = 0;

static void applyPageCommand(char cmd) {
  if (cmd == 'N') setDisplayPage(gDisplay, PAGE_NAVIGATION);
  else if (cmd == 'T') setDisplayPage(gDisplay, PAGE_TELEMETRY);
  else if (cmd == 'D') setDisplayPage(gDisplay, PAGE_DIAGNOSTICS);
  else if (cmd == 'A') setDisplayPage(gDisplay, PAGE_ABOUT);
  else if (cmd == 'S') {
    startDemo(gDemo, gState);
    setDisplayPage(gDisplay, PAGE_NAVIGATION);
  } else if (cmd == 'P') {
    pauseDemo(gDemo);
  } else if (cmd == 'R') {
    resetDemo(gDemo, gState);
    setDisplayPage(gDisplay, PAGE_NAVIGATION);
  }
}

#if ENABLE_TOUCH
static void readTouchControls() {
  TSPoint p = ts.getPoint();
  pinMode(TOUCH_XM, OUTPUT);
  pinMode(TOUCH_YP, OUTPUT);

  if (p.z < 100 || p.z > 1000) {
    return;
  }

  int16_t tx = map(p.y, TOUCH_MINY, TOUCH_MAXY, 0, gDisplay.width);
  int16_t ty = map(p.x, TOUCH_MINX, TOUCH_MAXX, 0, gDisplay.height);

  if (ty < gDisplay.height - 24) return;

  int16_t segment = gDisplay.width / 3;
  if (tx < segment) applyPageCommand('S');
  else if (tx < segment * 2) applyPageCommand('P');
  else applyPageCommand('R');
}
#endif

static void applyLinkStateTimeout(unsigned long nowMs) {
  if (!gState.demoMode && (nowMs - gSerial.lastPacketMs) > BACKEND_TIMEOUT_MS) {
    gState.backendConnected = false;
  }
}

static void drawTouchButtonsIfEnabled() {
#if ENABLE_TOUCH
  if (gDisplay.page != PAGE_NAVIGATION) return;
  MCUFRIEND_kbv &tft = gDisplay.tft;
  int16_t y = gDisplay.height - 22;
  int16_t w = gDisplay.width / 3;
  tft.fillRect(0, y, gDisplay.width, 22, COLOR_PANEL);
  tft.drawFastHLine(0, y, gDisplay.width, COLOR_PRIMARY);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_OK, COLOR_PANEL);
  tft.setCursor(w / 2 - 14, y + 8); tft.print("START");
  tft.setTextColor(COLOR_WARN, COLOR_PANEL);
  tft.setCursor(w + w / 2 - 14, y + 8); tft.print("PAUSE");
  tft.setTextColor(COLOR_ERROR, COLOR_PANEL);
  tft.setCursor(2 * w + w / 2 - 14, y + 8); tft.print("RESET");
#endif
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  initNavigationState(gState, MOCK_DATA_MODE);
  initSerialProtocol(gSerial);
  initDemoController(gDemo);

  initDisplay(gDisplay);
  showBootScreen(gDisplay);

  if (MOCK_DATA_MODE) {
    startDemo(gDemo, gState);
  }

  setDisplayPage(gDisplay, PAGE_NAVIGATION);
  drawCurrentPage(gDisplay, gState, true);
  drawTouchButtonsIfEnabled();

  Serial.println("IDR NAV READY");
  Serial.println("Commands: N/T/D/A pages, S/P/R demo control");
  Serial.println("Protocol: $IDR,SPEED=42.5,HEADING=127.2,GNSS=LOST,MODE=DR,CONF=87,LAT=52.4102,LON=-1.5603*");
}

void loop() {
  unsigned long nowMs = millis();

  if (readNavigationSerial(gSerial, gState)) {
    gState.demoMode = false;
    gState.backendConnected = true;
  }

  char command = consumeSerialCommand(gSerial);
  if (command) {
    applyPageCommand(command);
    drawTouchButtonsIfEnabled();
  }

#if ENABLE_TOUCH
  readTouchControls();
#endif

  if (gState.demoMode) {
    updateMockNavigation(gDemo, gState, nowMs);
  }

  applyLinkStateTimeout(nowMs);
  updateNavigationState(gState);

  const unsigned long uiInterval = 1000UL / UI_UPDATE_HZ;
  if (nowMs - gLastUiUpdateMs >= uiInterval) {
    drawCurrentPage(gDisplay, gState, false);
    drawTouchButtonsIfEnabled();
    gLastUiUpdateMs = nowMs;
  }
}
