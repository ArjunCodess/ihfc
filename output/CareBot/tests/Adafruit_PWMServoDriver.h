#pragma once
#include "Wire.h"

class Adafruit_PWMServoDriver {
public:
  explicit Adafruit_PWMServoDriver(uint8_t address) : address_(address) {}
  bool begin() { return pcaDevicePresent && !wireFail; }
  void setPWMFreq(float frequency) { frequency_ = frequency; }
  uint8_t readPrescale() const { return wireFail || frequency_ != 50 ? 0 : 121; }
  uint8_t setPWM(uint8_t channel, uint16_t on, uint16_t off) {
    Wire.beginTransmission(address_);
    Wire.write(uint8_t(0x06 + 4 * channel));
    Wire.write(uint8_t(on & 0xff));
    Wire.write(uint8_t(on >> 8));
    Wire.write(uint8_t(off & 0xff));
    Wire.write(uint8_t(off >> 8));
    return Wire.endTransmission() == 0 ? 0 : 1;
  }
  float frequency() const { return frequency_; }

private:
  uint8_t address_;
  float frequency_ = 0;
};
