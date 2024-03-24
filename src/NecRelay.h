#include <Arduino.h>

/// Class that can send unmodulated NEC commands.
class NecRelay {
public:
  NecRelay(uint8_t pin, bool activeLow);
  void SendCommand(uint8_t address, uint8_t command) const;

private:
  void SendByte(uint8_t byte) const;
  void SendPulse(bool value) const;
  const uint8_t pin;
  const uint8_t activeValue;
  const uint8_t nonActiveValue;
};
