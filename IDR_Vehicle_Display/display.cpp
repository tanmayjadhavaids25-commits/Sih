#include "display.h"

#include "config.h"

#if ENABLE_TOUCH
#include <TouchScreen.h>
#endif

#include <math.h>
#include <string.h>

struct MapRect {
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
};

static MapRect mapRect;

static uint16_t statusColor(IndicatorState state) {
  switch (state) {
    case IND_ACTIVE: return COLOR_OK;
    case IND_INACTIVE: return COLOR_TEXT_DIM;
    case IND_ERROR: return COLOR_ERROR;
    default: return COLOR_TEXT_DIM;
  }
}

static void writeLabel(MCUFRIEND_kbv &tft, int16_t x, int16_t y, const char *text, uint16_t color, uint8_t size) {
  tft.setTextColor(color, COLOR_PANEL);
  tft.setTextSize(size);
  tft.setCursor(x, y);
  tft.print(text);
}

static void fillRoundedCard(MCUFRIEND_kbv &tft, int16_t x, int16_t y, int16_t w, int16_t h) {
  tft.fillRoundRect(x, y, w, h, 6, COLOR_CARD);
  tft.drawRoundRect(x, y, w, h, 6, COLOR_PRIMARY);
}

static uint8_t headingToCardinal(float heading) {
  int16_t normalized = (int16_t)(heading + 0.5f) % 360;
  if (normalized < 0) normalized += 360;
  if (normalized < 45 || normalized >= 315) return 0;   // N
  if (normalized < 135) return 1;                        // E
  if (normalized < 225) return 2;                        // S
  return 3;                                              // W
}

void initDisplay(DisplayContext &display) {
  display.tftId = display.tft.readID();
  if (display.tftId == 0xD3D3 || display.tftId == 0xFFFF || display.tftId == 0x0000) {
    display.tftId = TFT_DRIVER_AUTO ? 0x9486 : TFT_DRIVER_MANUAL_ID;
  }
  if (!TFT_DRIVER_AUTO) {
    display.tftId = TFT_DRIVER_MANUAL_ID;
  }

  display.tft.begin(display.tftId);
  display.tft.setRotation(1);
  display.width = display.tft.width();
  display.height = display.tft.height();
  display.page = PAGE_NAVIGATION;
  display.staticDrawn = false;
  display.hasLastState = false;
  display.lastVehicleX = -1;
  display.lastVehicleY = -1;
  display.lastVehicleDir = 255;
  display.blinkMs = 0;
  display.blinkOn = true;

  mapRect.x = 10;
  mapRect.y = 40;
  mapRect.w = display.width - 20;
  mapRect.h = display.height / 3;
}

void showBootScreen(DisplayContext &display) {
  MCUFRIEND_kbv &tft = display.tft;
  tft.fillScreen(COLOR_BG);
  tft.setTextColor(COLOR_PRIMARY);
  tft.setTextSize(3);
  tft.setCursor(20, 28);
  tft.print("IDR NAV");

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(18, 70);
  tft.print("INTELLIGENT DEAD");
  tft.setCursor(18, 92);
  tft.print("RECKONING");

  tft.setTextColor(COLOR_TEXT_DIM);
  tft.setTextSize(1);
  tft.setCursor(28, 118);
  tft.print("GNSS-RESILIENT VEHICLE NAVIGATION");
  tft.setCursor(126, 132);
  tft.print("SIH 2026");

  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(18, 160);
  tft.print("INITIALIZING...");

  tft.setCursor(18, 180); tft.print("GPS "); tft.setTextColor(COLOR_OK); tft.print("\x03");
  tft.setCursor(18, 194); tft.setTextColor(COLOR_TEXT); tft.print("IMU "); tft.setTextColor(COLOR_OK); tft.print("\x03");
  tft.setCursor(18, 208); tft.setTextColor(COLOR_TEXT); tft.print("NAV ENGINE "); tft.setTextColor(COLOR_OK); tft.print("\x03");
  tft.setCursor(18, 222); tft.setTextColor(COLOR_TEXT); tft.print("DISPLAY "); tft.setTextColor(COLOR_OK); tft.print("\x03");

  delay(1200);
}

static void drawNavStatic(DisplayContext &display) {
  MCUFRIEND_kbv &tft = display.tft;
  tft.fillScreen(COLOR_BG);

  tft.fillRect(0, 0, display.width, 28, COLOR_PANEL);
  tft.drawFastHLine(0, 28, display.width, COLOR_PRIMARY);
  tft.setTextSize(2);
  tft.setTextColor(COLOR_PRIMARY, COLOR_PANEL);
  tft.setCursor(8, 7);
  tft.print("IDR NAV");

  drawNavigationMap(display, display.lastState, true);

  int16_t cardY = mapRect.y + mapRect.h + 8;
  int16_t cardW = (display.width - 24) / 3;
  fillRoundedCard(tft, 6, cardY, cardW, 54);
  fillRoundedCard(tft, 10 + cardW, cardY, cardW, 54);
  fillRoundedCard(tft, 14 + cardW * 2, cardY, cardW, 54);

  int16_t modeY = cardY + 62;
  fillRoundedCard(tft, 6, modeY, display.width - 12, display.height - modeY - 8);

  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD);
  tft.setTextSize(1);
  tft.setCursor(14, cardY + 6); tft.print("SPEED");
  tft.setCursor(18 + cardW, cardY + 6); tft.print("HEADING");
  tft.setCursor(20 + cardW * 2, cardY + 6); tft.print("CONFIDENCE");

  tft.setCursor(14, modeY + 6); tft.print("MODE");
  tft.setCursor(14, modeY + 28); tft.print("IMU");
  tft.setCursor(14, modeY + 42); tft.print("AI");
  tft.setCursor(14, modeY + 56); tft.print("EKF");
  tft.setCursor(88, modeY + 28); tft.print("NHC");
  tft.setCursor(88, modeY + 42); tft.print("MAP");

  tft.setCursor(display.width - 82, modeY + 6);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD);
  tft.print("10 Hz");
}

static void drawTelemetryStatic(DisplayContext &display) {
  MCUFRIEND_kbv &tft = display.tft;
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, display.width, 28, COLOR_PANEL);
  tft.drawFastHLine(0, 28, display.width, COLOR_PRIMARY);
  tft.setTextColor(COLOR_PRIMARY, COLOR_PANEL);
  tft.setTextSize(2);
  tft.setCursor(8, 7);
  tft.print("TELEMETRY");

  fillRoundedCard(tft, 8, 38, display.width - 16, display.height - 46);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD);
  tft.setCursor(16, 50); tft.print("SPEED");
  tft.setCursor(16, 76); tft.print("HEADING");
  tft.setCursor(16, 102); tft.print("GNSS");
  tft.setCursor(16, 128); tft.print("NAVIGATION");
  tft.setCursor(16, 154); tft.print("MAP CONFIDENCE");
  tft.setCursor(16, 180); tft.print("BATTERY");
  tft.setCursor(16, 206); tft.print("TEMPERATURE");
}

static void drawDiagnosticsStatic(DisplayContext &display) {
  MCUFRIEND_kbv &tft = display.tft;
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, display.width, 28, COLOR_PANEL);
  tft.drawFastHLine(0, 28, display.width, COLOR_PRIMARY);
  tft.setTextColor(COLOR_PRIMARY, COLOR_PANEL);
  tft.setTextSize(2);
  tft.setCursor(8, 7);
  tft.print("DIAGNOSTICS");

  fillRoundedCard(tft, 8, 38, display.width - 16, display.height - 46);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD);
  tft.setCursor(14, 48); tft.print("GNSS:");
  tft.setCursor(14, 62); tft.print("IMU:");
  tft.setCursor(14, 76); tft.print("AI VELOCITY:");
  tft.setCursor(14, 90); tft.print("EKF:");
  tft.setCursor(14, 104); tft.print("NHC:");
  tft.setCursor(14, 118); tft.print("MAP MATCH:");
  tft.setCursor(14, 132); tft.print("NAV RATE:");
  tft.setCursor(14, 146); tft.print("LAT:");
  tft.setCursor(14, 160); tft.print("LON:");
  tft.setCursor(14, 174); tft.print("SPEED:");
  tft.setCursor(14, 188); tft.print("HEADING:");
  tft.setCursor(14, 202); tft.print("CONFIDENCE:");
}

static void drawAboutStatic(DisplayContext &display) {
  MCUFRIEND_kbv &tft = display.tft;
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 0, display.width, 28, COLOR_PANEL);
  tft.drawFastHLine(0, 28, display.width, COLOR_PRIMARY);
  tft.setTextColor(COLOR_PRIMARY, COLOR_PANEL);
  tft.setTextSize(2);
  tft.setCursor(8, 7);
  tft.print("ABOUT");

  fillRoundedCard(tft, 8, 38, display.width - 16, display.height - 46);
  tft.setTextColor(COLOR_TEXT, COLOR_CARD);
  tft.setTextSize(2);
  tft.setCursor(16, 56);
  tft.print("IDR NAV");
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD);
  tft.setCursor(16, 82); tft.print("AI-Driven Intelligent Dead Reckoning");
  tft.setCursor(16, 98); tft.print("Vehicle Display Prototype");
  tft.setCursor(16, 114); tft.print("SIH 2026");
  tft.setCursor(16, 142); tft.print("HW: Arduino UNO + 3.5 TFT Shield");
  tft.setCursor(16, 156); tft.print("Backend: Raspberry Pi IDR Engine");
  tft.setCursor(16, 170); tft.print("Sensor feed: ESP32 GNSS + IMU");
  tft.setCursor(16, 198); tft.print("Serial pages: N/T/D/A");
  tft.setCursor(16, 212); tft.print("Demo controls: S/P/R");
}

void setDisplayPage(DisplayContext &display, DisplayPage page) {
  display.page = page;
  display.staticDrawn = false;
  display.lastVehicleX = -1;
  display.lastVehicleY = -1;
  display.lastVehicleDir = 255;
}

void drawNavigationMap(DisplayContext &display, const NavigationState &, bool fullRedraw) {
  MCUFRIEND_kbv &tft = display.tft;
  if (fullRedraw) {
    tft.fillRoundRect(mapRect.x, mapRect.y, mapRect.w, mapRect.h, 6, COLOR_PANEL);
    tft.drawRoundRect(mapRect.x, mapRect.y, mapRect.w, mapRect.h, 6, COLOR_PRIMARY);

    int16_t cx = mapRect.x + mapRect.w / 2;
    int16_t cy = mapRect.y + mapRect.h / 2;

    tft.drawLine(mapRect.x + 16, cy + 20, mapRect.x + mapRect.w - 18, cy - 20, COLOR_ROAD);
    tft.drawLine(mapRect.x + 28, cy + 26, mapRect.x + mapRect.w - 8, cy - 14, COLOR_ROAD);
    tft.drawLine(cx - 8, mapRect.y + 12, cx + 16, mapRect.y + mapRect.h - 8, COLOR_ROAD);

    tft.drawLine(mapRect.x + 20, cy + 16, mapRect.x + mapRect.w - 28, cy - 18, COLOR_ROUTE);
    tft.drawLine(mapRect.x + mapRect.w - 28, cy - 18, mapRect.x + mapRect.w - 14, cy - 30, COLOR_ROUTE);

    tft.fillCircle(mapRect.x + 48, mapRect.y + 30, 2, COLOR_TEXT_DIM);
    tft.fillCircle(mapRect.x + mapRect.w - 42, mapRect.y + mapRect.h - 24, 2, COLOR_TEXT_DIM);

    tft.setTextSize(1);
    tft.setTextColor(COLOR_TEXT_DIM, COLOR_PANEL);
    tft.setCursor(mapRect.x + 8, mapRect.y + 8);
    tft.print("NAVIGATION MAP");
  }
}

void drawVehicleMarker(DisplayContext &display, int16_t x, int16_t y, float heading, uint16_t color) {
  MCUFRIEND_kbv &tft = display.tft;
  uint8_t dir = headingToCardinal(heading);
  if (dir == 0) {
    tft.fillTriangle(x, y - 8, x - 6, y + 6, x + 6, y + 6, color);
  } else if (dir == 1) {
    tft.fillTriangle(x + 8, y, x - 6, y - 6, x - 6, y + 6, color);
  } else if (dir == 2) {
    tft.fillTriangle(x, y + 8, x - 6, y - 6, x + 6, y - 6, color);
  } else {
    tft.fillTriangle(x - 8, y, x + 6, y - 6, x + 6, y + 6, color);
  }
}

void updateVehiclePosition(DisplayContext &display, const NavigationState &state) {
  int32_t latNorm = (int32_t)((state.latitude - 52.4098f) * 120000.0f);
  int32_t lonNorm = (int32_t)((state.longitude + 1.5607f) * 120000.0f);

  int16_t x = mapRect.x + 16 + (lonNorm % (mapRect.w - 32));
  int16_t y = mapRect.y + mapRect.h - 18 - (latNorm % (mapRect.h - 26));
  if (x < mapRect.x + 8) x = mapRect.x + 8;
  if (x > mapRect.x + mapRect.w - 8) x = mapRect.x + mapRect.w - 8;
  if (y < mapRect.y + 8) y = mapRect.y + 8;
  if (y > mapRect.y + mapRect.h - 8) y = mapRect.y + mapRect.h - 8;

  uint8_t dir = headingToCardinal(state.heading);
  if (display.lastVehicleX >= 0) {
    drawVehicleMarker(display, display.lastVehicleX, display.lastVehicleY, display.lastVehicleDir * 90.0f, COLOR_PANEL);
    display.tft.drawCircle(display.lastVehicleX, display.lastVehicleY, 1, COLOR_ROUTE);
  }

  drawVehicleMarker(display, x, y, state.heading, COLOR_PRIMARY);
  display.lastVehicleX = x;
  display.lastVehicleY = y;
  display.lastVehicleDir = dir;
}

void drawHeadingArrow(DisplayContext &display, float heading) {
  int16_t x = mapRect.x + mapRect.w - 20;
  int16_t y = mapRect.y + 20;
  display.tft.fillRect(x - 10, y - 10, 20, 20, COLOR_PANEL);
  drawVehicleMarker(display, x, y, heading, COLOR_WARN);
}

void drawSpeed(DisplayContext &display, float speed) {
  char value[12];
  dtostrf(speed, 4, 0, value);
  display.tft.fillRect(14, mapRect.y + mapRect.h + 24, (display.width - 24) / 3 - 16, 26, COLOR_CARD);
  display.tft.setTextColor(COLOR_TEXT, COLOR_CARD);
  display.tft.setTextSize(2);
  display.tft.setCursor(16, mapRect.y + mapRect.h + 28);
  display.tft.print(value);
  display.tft.setTextSize(1);
  display.tft.setCursor(16, mapRect.y + mapRect.h + 46);
  display.tft.print(SPEED_UNIT);
}

void drawHeading(DisplayContext &display, float heading) {
  char value[12];
  dtostrf(heading, 3, 0, value);
  int16_t x = 10 + (display.width - 24) / 3;
  display.tft.fillRect(x + 8, mapRect.y + mapRect.h + 24, (display.width - 24) / 3 - 16, 26, COLOR_CARD);
  display.tft.setTextColor(COLOR_TEXT, COLOR_CARD);
  display.tft.setTextSize(2);
  display.tft.setCursor(x + 10, mapRect.y + mapRect.h + 28);
  display.tft.print(value);
  display.tft.print("\xF8");
}

void drawConfidence(DisplayContext &display, float confidence) {
  int16_t x = 14 + ((display.width - 24) / 3) * 2;
  int16_t width = (display.width - 24) / 3 - 18;
  int16_t barW = (int16_t)((confidence / 100.0f) * (float)width);
  if (barW < 0) barW = 0;
  if (barW > width) barW = width;

  display.tft.fillRect(x, mapRect.y + mapRect.h + 30, width, 8, COLOR_PANEL);
  display.tft.fillRect(x, mapRect.y + mapRect.h + 30, barW, 8, COLOR_OK);
  display.tft.drawRect(x, mapRect.y + mapRect.h + 30, width, 8, COLOR_TEXT_DIM);

  char value[8];
  itoa((int)confidence, value, 10);
  display.tft.setTextColor(COLOR_TEXT, COLOR_CARD);
  display.tft.setTextSize(1);
  display.tft.fillRect(x, mapRect.y + mapRect.h + 42, width, 8, COLOR_CARD);
  display.tft.setCursor(x, mapRect.y + mapRect.h + 42);
  display.tft.print(value);
  display.tft.print('%');
}

static void drawStatusDot(MCUFRIEND_kbv &tft, int16_t x, int16_t y, IndicatorState state) {
  tft.fillCircle(x, y, 4, statusColor(state));
}

static void drawBanner(DisplayContext &display, const NavigationState &state) {
  MCUFRIEND_kbv &tft = display.tft;
  uint16_t color = COLOR_OK;
  const char *prefix = "GNSS ";
  if (state.gnssState == GNSS_LOST || state.gnssState == GNSS_NO_FIX) color = COLOR_ERROR;
  if (state.gnssState == GNSS_RESTORED) color = COLOR_WARN;

  tft.fillRect(display.width - 158, 4, 154, 20, COLOR_PANEL);
  tft.setTextSize(1);
  tft.setTextColor(color, COLOR_PANEL);
  tft.setCursor(display.width - 154, 10);
  tft.print("\x07 ");
  tft.print(prefix);
  tft.print(gnssStateLabel(state.gnssState));
}

static void drawModeSection(DisplayContext &display, const NavigationState &state) {
  MCUFRIEND_kbv &tft = display.tft;
  int16_t modeY = mapRect.y + mapRect.h + 8 + 62;

  tft.fillRect(52, modeY + 4, display.width - 64, 14, COLOR_CARD);
  tft.setTextSize(1);
  if (state.mode == NAV_MODE_DEAD_RECKONING) tft.setTextColor(COLOR_WARN, COLOR_CARD);
  else if (state.mode == NAV_MODE_RESTORING) tft.setTextColor(COLOR_PRIMARY, COLOR_CARD);
  else tft.setTextColor(COLOR_TEXT, COLOR_CARD);
  tft.setCursor(52, modeY + 7);
  tft.print(navigationModeLabel(state.mode));

  drawStatusDot(tft, 56, modeY + 31, state.imuState);
  drawStatusDot(tft, 56, modeY + 45, state.aiState);
  drawStatusDot(tft, 56, modeY + 59, state.ekfState);
  drawStatusDot(tft, 126, modeY + 31, state.nhcState);
  drawStatusDot(tft, 126, modeY + 45, state.mapState);

  tft.fillRect(display.width - 132, modeY + 22, 122, 42, COLOR_CARD);
  tft.setTextSize(1);
  tft.setCursor(display.width - 130, modeY + 24);
  if (state.demoMode) {
    tft.setTextColor(COLOR_WARN, COLOR_CARD);
    tft.print("DEMO / SIMULATED");
  } else {
    tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD);
    tft.print("LIVE INPUT");
  }

  if (!state.backendConnected && !state.demoMode) {
    tft.setTextColor(COLOR_ERROR, COLOR_CARD);
    tft.setCursor(display.width - 130, modeY + 38);
    tft.print("BACKEND NOT CONNECTED");
  } else {
    tft.fillRect(display.width - 130, modeY + 36, 118, 10, COLOR_CARD);
  }

  if (state.gnssState == GNSS_LOST) {
    tft.setTextColor(COLOR_WARN, COLOR_CARD);
    tft.setCursor(display.width - 130, modeY + 52);
    tft.print("INTELLIGENT DR ACTIVE");
  } else if (state.gnssState == GNSS_RESTORED) {
    tft.setTextColor(COLOR_OK, COLOR_CARD);
    tft.setCursor(display.width - 130, modeY + 52);
    tft.print("RE-SYNCHRONIZING");
  } else {
    tft.fillRect(display.width - 130, modeY + 50, 118, 10, COLOR_CARD);
  }
}

static void drawTelemetryPage(DisplayContext &display, const NavigationState &state) {
  MCUFRIEND_kbv &tft = display.tft;
  char text[24];

  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT, COLOR_CARD);
  tft.fillRect(130, 47, 170, 20, COLOR_CARD);
  dtostrf(state.speed, 4, 0, text);
  tft.setCursor(130, 50); tft.print(text); tft.print(' '); tft.print(SPEED_UNIT);

  tft.fillRect(130, 73, 160, 20, COLOR_CARD);
  dtostrf(state.heading, 3, 0, text);
  tft.setCursor(130, 76); tft.print(text); tft.print("\xF8");

  tft.fillRect(130, 99, 160, 20, COLOR_CARD);
  tft.setCursor(130, 102);
  tft.setTextColor((state.gnssAvailable ? COLOR_OK : COLOR_ERROR), COLOR_CARD);
  tft.print(gnssStateLabel(state.gnssState));

  tft.fillRect(130, 125, 180, 20, COLOR_CARD);
  tft.setTextColor(COLOR_TEXT, COLOR_CARD);
  tft.setCursor(130, 128); tft.print(navigationModeLabel(state.mode));

  tft.fillRect(130, 151, 160, 20, COLOR_CARD);
  itoa(state.mapConfidence, text, 10);
  tft.setCursor(130, 154); tft.print(text); tft.print('%');

  tft.fillRect(130, 177, 160, 20, COLOR_CARD);
  dtostrf(state.batteryPercent, 4, 0, text);
  tft.setCursor(130, 180); tft.print(text); tft.print('%');

  tft.fillRect(130, 203, 160, 20, COLOR_CARD);
  dtostrf(state.temperatureC, 4, 1, text);
  tft.setCursor(130, 206); tft.print(text); tft.print("C");

  if (state.demoMode) {
    tft.setTextSize(1);
    tft.setTextColor(COLOR_WARN, COLOR_CARD);
    tft.setCursor(210, 220);
    tft.print("DEMO / SIMULATED");
  }
}

static void drawDiagnosticsPage(DisplayContext &display, const NavigationState &state) {
  MCUFRIEND_kbv &tft = display.tft;
  char text[26];

  tft.setTextSize(1);

  tft.fillRect(110, 46, 190, 160, COLOR_CARD);

  tft.setTextColor(state.gnssAvailable ? COLOR_OK : COLOR_ERROR, COLOR_CARD);
  tft.setCursor(110, 48); tft.print(gnssStateLabel(state.gnssState));

  tft.setTextColor(statusColor(state.imuState), COLOR_CARD);
  tft.setCursor(110, 62); tft.print(state.imuState == IND_ACTIVE ? "ACTIVE" : (state.imuState == IND_ERROR ? "ERROR" : "INACTIVE"));

  tft.setTextColor(statusColor(state.aiState), COLOR_CARD);
  tft.setCursor(110, 76); tft.print(state.aiState == IND_ACTIVE ? "ACTIVE" : (state.aiState == IND_ERROR ? "ERROR" : "INACTIVE"));

  tft.setTextColor(statusColor(state.ekfState), COLOR_CARD);
  tft.setCursor(110, 90); tft.print(state.ekfState == IND_ACTIVE ? "ACTIVE" : (state.ekfState == IND_ERROR ? "ERROR" : "INACTIVE"));

  tft.setTextColor(statusColor(state.nhcState), COLOR_CARD);
  tft.setCursor(110, 104); tft.print(state.nhcState == IND_ACTIVE ? "ACTIVE" : (state.nhcState == IND_ERROR ? "ERROR" : "INACTIVE"));

  tft.setTextColor(statusColor(state.mapState), COLOR_CARD);
  tft.setCursor(110, 118); tft.print(state.mapState == IND_ACTIVE ? "ACTIVE" : (state.mapState == IND_ERROR ? "ERROR" : "INACTIVE"));

  tft.setTextColor(COLOR_TEXT, COLOR_CARD);
  tft.setCursor(110, 132); tft.print("10 Hz");

  dtostrf(state.latitude, 2, 6, text);
  tft.setCursor(110, 146); tft.print(text);

  dtostrf(state.longitude, 2, 6, text);
  tft.setCursor(110, 160); tft.print(text);

  dtostrf(state.speed, 4, 1, text);
  tft.setCursor(110, 174); tft.print(text); tft.print(' '); tft.print(SPEED_UNIT);

  dtostrf(state.heading, 4, 1, text);
  tft.setCursor(110, 188); tft.print(text); tft.print("\xF8");

  itoa(state.mapConfidence, text, 10);
  tft.setCursor(110, 202); tft.print(text); tft.print('%');
}

void drawCurrentPage(DisplayContext &display, const NavigationState &state, bool forceStatic) {
  if (forceStatic || !display.staticDrawn) {
    if (display.page == PAGE_NAVIGATION) drawNavStatic(display);
    else if (display.page == PAGE_TELEMETRY) drawTelemetryStatic(display);
    else if (display.page == PAGE_DIAGNOSTICS) drawDiagnosticsStatic(display);
    else drawAboutStatic(display);
    display.staticDrawn = true;
    display.hasLastState = false;
  }

  if (display.page == PAGE_NAVIGATION) {
    drawBanner(display, state);
    drawSpeed(display, state.speed);
    drawHeading(display, state.heading);
    drawConfidence(display, (float)state.mapConfidence);
    drawHeadingArrow(display, state.heading);
    updateVehiclePosition(display, state);
    drawModeSection(display, state);
  } else if (display.page == PAGE_TELEMETRY) {
    drawTelemetryPage(display, state);
  } else if (display.page == PAGE_DIAGNOSTICS) {
    drawDiagnosticsPage(display, state);
  }

  display.lastState = state;
  display.hasLastState = true;
}
