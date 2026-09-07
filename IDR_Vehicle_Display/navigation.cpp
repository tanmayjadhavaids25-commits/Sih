#include "navigation.h"

static float wrapHeading(float heading) {
  while (heading < 0.0f) heading += 360.0f;
  while (heading >= 360.0f) heading -= 360.0f;
  return heading;
}

void initNavigationState(NavigationState &state, bool demoMode) {
  state.latitude = 52.4102f;
  state.longitude = -1.5603f;
  state.speed = 42.0f;
  state.heading = 127.0f;
  state.accuracy = 1.8f;
  state.mapConfidence = 87;

  state.gnssAvailable = true;
  state.imuActive = true;
  state.aiActive = true;
  state.ekfActive = true;
  state.nhcActive = true;
  state.mapMatchingActive = true;

  state.mode = demoMode ? NAV_MODE_DEMO : NAV_MODE_GNSS_INS;
  state.timestamp = millis();
  state.demoMode = demoMode;

  state.gnssState = GNSS_AVAILABLE;
  state.backendConnected = demoMode;

  state.imuState = IND_ACTIVE;
  state.aiState = IND_ACTIVE;
  state.ekfState = IND_ACTIVE;
  state.nhcState = IND_ACTIVE;
  state.mapState = IND_ACTIVE;

  state.batteryPercent = 74.0f;
  state.temperatureC = 31.0f;
  state.estimatedRangeKm = 182.0f;
}

void normalizeNavigationState(NavigationState &state) {
  if (state.speed < 0.0f) state.speed = 0.0f;
  state.heading = wrapHeading(state.heading);
  if (state.mapConfidence < 0) state.mapConfidence = 0;
  if (state.mapConfidence > 100) state.mapConfidence = 100;
  if (state.accuracy < 0.0f) state.accuracy = 0.0f;
}

void updateNavigationState(NavigationState &state) {
  normalizeNavigationState(state);

  state.imuState = state.imuActive ? IND_ACTIVE : IND_INACTIVE;
  state.aiState = state.aiActive ? IND_ACTIVE : IND_INACTIVE;
  state.ekfState = state.ekfActive ? IND_ACTIVE : IND_INACTIVE;
  state.nhcState = state.nhcActive ? IND_ACTIVE : IND_INACTIVE;
  state.mapState = state.mapMatchingActive ? IND_ACTIVE : IND_INACTIVE;

  if (state.gnssState == GNSS_NO_FIX) {
    state.gnssAvailable = false;
  }

  if (state.gnssState == GNSS_LOST || !state.gnssAvailable) {
    state.mode = NAV_MODE_DEAD_RECKONING;
  } else if (state.gnssState == GNSS_RESTORED) {
    state.mode = NAV_MODE_RESTORING;
  } else if (!state.demoMode) {
    state.mode = NAV_MODE_GNSS_INS;
  } else if (state.mode != NAV_MODE_RESTORING) {
    state.mode = NAV_MODE_DEMO;
  }

  state.timestamp = millis();
}

const char *navigationModeLabel(NavigationMode mode) {
  switch (mode) {
    case NAV_MODE_GNSS_INS: return "GNSS + INS";
    case NAV_MODE_DEAD_RECKONING: return "DEAD RECKONING";
    case NAV_MODE_RESTORING: return "RE-SYNCHRONIZING";
    case NAV_MODE_DEMO: return "DEMO";
    default: return "UNKNOWN";
  }
}

const char *gnssStateLabel(GnssState state) {
  switch (state) {
    case GNSS_AVAILABLE: return "AVAILABLE";
    case GNSS_LOST: return "LOST";
    case GNSS_RESTORED: return "RESTORED";
    case GNSS_NO_FIX: return "NO FIX";
    default: return "UNKNOWN";
  }
}

const char *gnssBannerLabel(GnssState state) {
  switch (state) {
    case GNSS_AVAILABLE: return "GNSS AVAILABLE";
    case GNSS_LOST: return "GNSS SIGNAL LOST";
    case GNSS_RESTORED: return "GNSS RESTORED";
    case GNSS_NO_FIX: return "GNSS NO FIX";
    default: return "GNSS UNKNOWN";
  }
}
