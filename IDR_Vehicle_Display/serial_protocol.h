#ifndef IDR_SERIAL_PROTOCOL_H
#define IDR_SERIAL_PROTOCOL_H

#include <Arduino.h>
#include "navigation.h"

struct SerialProtocolContext {
  char buffer[180];
  uint8_t index;
  bool hasNewPacket;
  char pendingCommand;
  unsigned long lastPacketMs;
};

void initSerialProtocol(SerialProtocolContext &ctx);
bool readNavigationSerial(SerialProtocolContext &ctx, NavigationState &state);
bool parseNavigationData(const char *line, NavigationState &state);
char consumeSerialCommand(SerialProtocolContext &ctx);

#endif
