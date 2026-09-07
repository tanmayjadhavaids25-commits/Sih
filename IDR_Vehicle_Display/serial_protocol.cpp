#include "serial_protocol.h"

#include <stdlib.h>
#include <string.h>

static bool startsWith(const char *line, const char *prefix) {
  while (*prefix) {
    if (*line++ != *prefix++) return false;
  }
  return true;
}

static void applyField(char *field, NavigationState &state) {
  char *eq = strchr(field, '=');
  if (!eq) return;
  *eq = '\0';
  const char *key = field;
  const char *value = eq + 1;

  if (strcmp(key, "SPEED") == 0) {
    state.speed = atof(value);
  } else if (strcmp(key, "HEADING") == 0) {
    state.heading = atof(value);
  } else if (strcmp(key, "GNSS") == 0) {
    if (strcmp(value, "AVAILABLE") == 0) {
      state.gnssState = GNSS_AVAILABLE;
      state.gnssAvailable = true;
    } else if (strcmp(value, "LOST") == 0) {
      state.gnssState = GNSS_LOST;
      state.gnssAvailable = false;
    } else if (strcmp(value, "RESTORED") == 0) {
      state.gnssState = GNSS_RESTORED;
      state.gnssAvailable = true;
    } else if (strcmp(value, "NOFIX") == 0) {
      state.gnssState = GNSS_NO_FIX;
      state.gnssAvailable = false;
    }
  } else if (strcmp(key, "MODE") == 0) {
    if (strcmp(value, "GNSS_INS") == 0) state.mode = NAV_MODE_GNSS_INS;
    else if (strcmp(value, "DR") == 0) state.mode = NAV_MODE_DEAD_RECKONING;
    else if (strcmp(value, "RESTORING") == 0) state.mode = NAV_MODE_RESTORING;
    else if (strcmp(value, "DEMO") == 0) state.mode = NAV_MODE_DEMO;
  } else if (strcmp(key, "CONF") == 0) {
    state.mapConfidence = atoi(value);
  } else if (strcmp(key, "LAT") == 0) {
    state.latitude = atof(value);
  } else if (strcmp(key, "LON") == 0) {
    state.longitude = atof(value);
  } else if (strcmp(key, "ACC") == 0) {
    state.accuracy = atof(value);
  } else if (strcmp(key, "IMU") == 0) {
    state.imuActive = strcmp(value, "1") == 0 || strcmp(value, "ACTIVE") == 0;
  } else if (strcmp(key, "AI") == 0) {
    state.aiActive = strcmp(value, "1") == 0 || strcmp(value, "ACTIVE") == 0;
  } else if (strcmp(key, "EKF") == 0) {
    state.ekfActive = strcmp(value, "1") == 0 || strcmp(value, "ACTIVE") == 0;
  } else if (strcmp(key, "NHC") == 0) {
    state.nhcActive = strcmp(value, "1") == 0 || strcmp(value, "ACTIVE") == 0;
  } else if (strcmp(key, "MAP") == 0) {
    state.mapMatchingActive = strcmp(value, "1") == 0 || strcmp(value, "ACTIVE") == 0;
  } else if (strcmp(key, "BATT") == 0) {
    state.batteryPercent = atof(value);
  } else if (strcmp(key, "TEMP") == 0) {
    state.temperatureC = atof(value);
  } else if (strcmp(key, "RANGE") == 0) {
    state.estimatedRangeKm = atof(value);
  }
}

void initSerialProtocol(SerialProtocolContext &ctx) {
  ctx.index = 0;
  ctx.buffer[0] = '\0';
  ctx.hasNewPacket = false;
  ctx.pendingCommand = 0;
  ctx.lastPacketMs = 0;
}

bool parseNavigationData(const char *line, NavigationState &state) {
  if (!startsWith(line, "$IDR,")) return false;

  char temp[180];
  strncpy(temp, line + 5, sizeof(temp) - 1);
  temp[sizeof(temp) - 1] = '\0';

  char *star = strchr(temp, '*');
  if (star) *star = '\0';

  char *token = strtok(temp, ",");
  while (token != NULL) {
    applyField(token, state);
    token = strtok(NULL, ",");
  }
  state.backendConnected = true;
  state.demoMode = false;
  return true;
}

char consumeSerialCommand(SerialProtocolContext &ctx) {
  char c = ctx.pendingCommand;
  ctx.pendingCommand = 0;
  return c;
}

bool readNavigationSerial(SerialProtocolContext &ctx, NavigationState &state) {
  bool parsed = false;

  while (Serial.available()) {
    char c = (char)Serial.read();

    if (c == '\r') continue;

    if (c == '\n') {
      ctx.buffer[ctx.index] = '\0';
      if (ctx.index == 1) {
        char cmd = ctx.buffer[0];
        if (cmd >= 'a' && cmd <= 'z') cmd -= 32;
        if (cmd == 'N' || cmd == 'T' || cmd == 'D' || cmd == 'A' || cmd == 'S' || cmd == 'P' || cmd == 'R') {
          ctx.pendingCommand = cmd;
        }
      } else if (ctx.index > 0 && startsWith(ctx.buffer, "$IDR,")) {
        parsed = parseNavigationData(ctx.buffer, state);
        if (parsed) {
          ctx.lastPacketMs = millis();
        }
      }
      ctx.index = 0;
      ctx.buffer[0] = '\0';
      continue;
    }

    if (ctx.index < sizeof(ctx.buffer) - 1) {
      ctx.buffer[ctx.index++] = c;
    }
  }

  return parsed;
}
