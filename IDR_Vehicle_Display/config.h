#ifndef IDR_CONFIG_H
#define IDR_CONFIG_H

#include <Arduino.h>

// ============================
// Build / behavior configuration
// ============================
#define SERIAL_BAUD_RATE 115200
#define MOCK_DATA_MODE true
#define ENABLE_TOUCH false
#define UI_UPDATE_HZ 10
#define BACKEND_TIMEOUT_MS 2500UL

// ============================
// TFT configuration
// ============================
// If true, controller ID is detected with tft.readID().
#define TFT_DRIVER_AUTO true
// Used only if TFT_DRIVER_AUTO is false.
#define TFT_DRIVER_MANUAL_ID 0x9486

// Optional pin labels for shields/wiring documentation.
// Typical 3.5" UNO shield control pins:
#define TFT_CS A3
#define TFT_CD A2
#define TFT_WR A1
#define TFT_RD A0
#define TFT_RST A4

// Touch controller defaults for many MCUFRIEND shields.
#define TOUCH_YP A3
#define TOUCH_XM A2
#define TOUCH_YM 9
#define TOUCH_XP 8
#define TOUCH_MINX 120
#define TOUCH_MAXX 900
#define TOUCH_MINY 70
#define TOUCH_MAXY 920

// ============================
// Colors (RGB565)
// ============================
static const uint16_t COLOR_BG = 0x0821;        // dark navy
static const uint16_t COLOR_PANEL = 0x1043;     // dark panel
static const uint16_t COLOR_CARD = 0x18A3;      // card background
static const uint16_t COLOR_PRIMARY = 0x05FF;   // cyan
static const uint16_t COLOR_TEXT = 0xFFFF;      // white
static const uint16_t COLOR_TEXT_DIM = 0xBDF7;  // light gray
static const uint16_t COLOR_OK = 0x07E0;        // green
static const uint16_t COLOR_WARN = 0xFD20;      // amber
static const uint16_t COLOR_ERROR = 0xF800;     // red
static const uint16_t COLOR_ROUTE = 0x03EF;     // blue-cyan route
static const uint16_t COLOR_ROAD = 0x39E7;      // muted road

static const char SPEED_UNIT[] = "km/h";

enum NavigationMode {
  NAV_MODE_GNSS_INS = 0,
  NAV_MODE_DEAD_RECKONING = 1,
  NAV_MODE_RESTORING = 2,
  NAV_MODE_DEMO = 3
};

enum GnssState {
  GNSS_AVAILABLE = 0,
  GNSS_LOST = 1,
  GNSS_RESTORED = 2,
  GNSS_NO_FIX = 3
};

enum IndicatorState {
  IND_ACTIVE = 0,
  IND_INACTIVE = 1,
  IND_ERROR = 2
};

enum DisplayPage {
  PAGE_NAVIGATION = 0,
  PAGE_TELEMETRY = 1,
  PAGE_DIAGNOSTICS = 2,
  PAGE_ABOUT = 3
};

#endif
