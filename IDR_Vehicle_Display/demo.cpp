#include "demo.h"

#include <math.h>

static const uint8_t ROUTE_POINTS = 12;
static const float ROUTE_LAT[ROUTE_POINTS] = {
  52.41020f, 52.41026f, 52.41034f, 52.41042f,
  52.41050f, 52.41060f, 52.41066f, 52.41068f,
  52.41063f, 52.41054f, 52.41043f, 52.41033f
};
static const float ROUTE_LON[ROUTE_POINTS] = {
  -1.56030f, -1.56022f, -1.56010f, -1.55995f,
  -1.55980f, -1.55970f, -1.55974f, -1.55988f,
  -1.56003f, -1.56013f, -1.56020f, -1.56026f
};

static float toDegrees(float rad) {
  return rad * 57.2957795f;
}

static void advanceRoute(DemoController &demo, NavigationState &state) {
  uint16_t current = demo.routeIndex;
  uint16_t next = (uint16_t)((current + 1) % ROUTE_POINTS);

  float dLat = ROUTE_LAT[next] - ROUTE_LAT[current];
  float dLon = ROUTE_LON[next] - ROUTE_LON[current];
  float heading = toDegrees(atan2f(dLon, dLat));
  if (heading < 0.0f) heading += 360.0f;

  state.latitude = ROUTE_LAT[next];
  state.longitude = ROUTE_LON[next];
  state.heading = heading;

  if (state.mode == NAV_MODE_DEAD_RECKONING) {
    state.speed = 39.0f;
    state.mapConfidence -= 1;
    if (state.mapConfidence < 62) state.mapConfidence = 62;
  } else if (state.mode == NAV_MODE_RESTORING) {
    state.speed = 36.0f;
    state.mapConfidence += 2;
    if (state.mapConfidence > 85) state.mapConfidence = 85;
  } else {
    state.speed = 42.0f;
    state.mapConfidence += 1;
    if (state.mapConfidence > 90) state.mapConfidence = 90;
  }

  demo.routeIndex = next;
}

void initDemoController(DemoController &demo) {
  demo.running = false;
  demo.paused = false;
  demo.stage = 0;
  demo.stageStartMs = 0;
  demo.lastMoveMs = 0;
  demo.routeIndex = 0;
}

void startDemo(DemoController &demo, NavigationState &state) {
  demo.running = true;
  demo.paused = false;
  demo.stage = 0;
  demo.stageStartMs = millis();
  demo.lastMoveMs = 0;
  state.demoMode = true;
  state.backendConnected = true;
  state.gnssState = GNSS_AVAILABLE;
  state.gnssAvailable = true;
  state.mode = NAV_MODE_DEMO;
  state.mapConfidence = 87;
}

void pauseDemo(DemoController &demo) {
  if (!demo.running) return;
  demo.paused = !demo.paused;
}

void resetDemo(DemoController &demo, NavigationState &state) {
  initDemoController(demo);
  state.demoMode = true;
  state.gnssState = GNSS_AVAILABLE;
  state.gnssAvailable = true;
  state.mode = NAV_MODE_DEMO;
  state.mapConfidence = 87;
  state.speed = 42.0f;
  state.heading = 127.0f;
  state.latitude = ROUTE_LAT[0];
  state.longitude = ROUTE_LON[0];
}

void updateMockNavigation(DemoController &demo, NavigationState &state, unsigned long nowMs) {
  if (!demo.running || demo.paused) {
    return;
  }

  unsigned long elapsed = nowMs - demo.stageStartMs;

  if (demo.stage == 0 && elapsed > 8000UL) {
    demo.stage = 1;
    demo.stageStartMs = nowMs;
    state.gnssState = GNSS_LOST;
    state.gnssAvailable = false;
    state.mode = NAV_MODE_DEAD_RECKONING;
  } else if (demo.stage == 1 && elapsed > 9000UL) {
    demo.stage = 2;
    demo.stageStartMs = nowMs;
    state.gnssState = GNSS_RESTORED;
    state.gnssAvailable = true;
    state.mode = NAV_MODE_RESTORING;
  } else if (demo.stage == 2 && elapsed > 4000UL) {
    demo.stage = 3;
    demo.stageStartMs = nowMs;
    state.gnssState = GNSS_AVAILABLE;
    state.gnssAvailable = true;
    state.mode = NAV_MODE_DEMO;
  } else if (demo.stage == 3 && elapsed > 9000UL) {
    demo.stage = 0;
    demo.stageStartMs = nowMs;
    state.gnssState = GNSS_AVAILABLE;
    state.gnssAvailable = true;
    state.mode = NAV_MODE_DEMO;
    state.mapConfidence = 88;
  }

  if (nowMs - demo.lastMoveMs >= 350UL) {
    advanceRoute(demo, state);
    demo.lastMoveMs = nowMs;
  }
}
