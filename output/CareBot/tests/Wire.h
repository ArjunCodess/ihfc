#pragma once
#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

extern bool wireFail;
extern bool pcaDevicePresent;
extern int wireSda;
extern int wireScl;
extern std::map<int, int> servoTicks;

struct TwoWire {
  uint8_t address = 0;
  std::vector<uint8_t> bytes;

  void begin(int sda, int scl) { wireSda = sda; wireScl = scl; }
  void beginTransmission(uint8_t target) {
    address = target;
    bytes.clear();
  }
  size_t write(uint8_t value) {
    bytes.push_back(value);
    return 1;
  }
  uint8_t endTransmission() {
    if (wireFail || (address == 0x40 && !pcaDevicePresent)) return 4;
    if (address == 0x40 && bytes.size() == 5 && bytes[0] >= 0x06) {
      int channel = (bytes[0] - 0x06) / 4;
      int off = bytes[3] | ((bytes[4] & 0x0f) << 8);
      servoTicks[channel] = off;
    }
    return 0;
  }
};

extern TwoWire Wire;
