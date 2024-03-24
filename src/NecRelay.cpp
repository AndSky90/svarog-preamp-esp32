#include <Arduino.h>
#include "NecRelay.h"

#define NEC_VALUE_PULSE_DURATION 100
#define NEC_0_PAUSE_DURATION 100
#define NEC_1_PAUSE_DURATION 300

NecRelay::NecRelay(uint8_t pin, bool activeLow)
    : pin(pin),
      activeValue(activeLow ? LOW : HIGH),
      nonActiveValue(activeLow ? HIGH : LOW)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, nonActiveValue);
}

void NecRelay::SendPulse(bool value) const {
  digitalWrite(pin, activeValue);
  delayMicroseconds(NEC_VALUE_PULSE_DURATION);
  digitalWrite(pin, nonActiveValue);

  if (value) {
    delayMicroseconds(NEC_1_PAUSE_DURATION);
  } else {
    delayMicroseconds(NEC_0_PAUSE_DURATION);
  }
}

void NecRelay::SendByte(uint8_t byte) const {
  for (int bit = 0; bit < 8; bit++) {
    SendPulse((byte >> bit) & 1);
  }
}

void NecRelay::SendCommand(uint8_t address, uint8_t command) const {
  //ATOMIC() 
  {
    digitalWrite(pin, activeValue);
    delayMicroseconds(1600);
    digitalWrite(pin, nonActiveValue);
    delayMicroseconds(800);
    SendByte(address);
    SendByte(command);
    //SendPulse(false);
    digitalWrite(pin, nonActiveValue);
  }
}