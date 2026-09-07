#ifndef IDR_NAVIGATION_H
#define IDR_NAVIGATION_H

#include <Arduino.h>
#include "config.h"

struct NavigationState {
  float latitude;
  float longitude;
  float speed;
  float heading;
  float accuracy;
  int mapConfidence;

  bool gnssAvailable;
  bool imuActive;
  bool aiActive;
  bool ekfActive;
  bool nhcActive;
  bool mapMatchingActive;

  NavigationMode mode;
  unsigned long timestamp;
  bool demoMode;

  GnssState gnssState;
  bool backendConnected;

  IndicatorState imuState;
  IndicatorState aiState;
  IndicatorState ekfState;
  IndicatorState nhcState;
  IndicatorState mapState;

  float batteryPercent;
  float temperatureC;
  float estimatedRangeKm;
};

void initNavigationState(NavigationState &state, bool demoMode);
void normalizeNavigationState(NavigationState &state);
void updateNavigationState(NavigationState &state);

const char *navigationModeLabel(NavigationMode mode);
const char *gnssStateLabel(GnssState state);
const char *gnssBannerLabel(GnssState state);

#endif
