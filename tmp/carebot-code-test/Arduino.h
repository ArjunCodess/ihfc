#pragma once
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <deque>
#include <map>
#include <string>
#include <functional>
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define CONFIG_IDF_TARGET_ESP32 1
extern uint32_t tick;
extern std::deque<uint32_t> echoes;
extern std::map<int,int> duties;
extern std::map<int,int> levels;
extern bool lineMode;
extern uint32_t movingMs;
extern int failingWritePin;
extern std::function<int(int)> readHook;
extern std::function<void(std::string)> logHook;
extern std::function<void()> timeHook;
inline void advance(uint32_t ms) {
  if(duties[25] || duties[26] || duties[27] || duties[14]) movingMs+=ms;
  tick+=ms;
  if(timeHook) timeHook();
}
struct SerialMock {
  std::deque<char> input;
  void begin(int) {}
  int available() { return input.size(); }
  char read() { char c=input.front(); input.pop_front(); return c; }
  template<class T> void print(T) {}
  template<class T> void println(T) {}
  void println(const char *s) {if(logHook) logHook(s);}
};
extern SerialMock Serial;
template<class T> T constrain(T x,T lo,T hi) {return std::max(lo,std::min(hi,x));}
inline uint32_t millis() { return tick; }
inline void delay(uint32_t n) { advance(n); }
inline void delayMicroseconds(uint32_t) {}
inline void pinMode(int,int) {}
inline void digitalWrite(int pin,int level) {levels[pin]=level;}
inline int digitalRead(int pin) {
  if(readHook) return readHook(pin);
  if(pin==35) return lineMode && tick>=200 ? LOW : HIGH;
  return HIGH;
}
inline uint32_t pulseIn(int,int,uint32_t timeout) {
  uint32_t v=5831;
  if(!echoes.empty()) {v=echoes.front(); echoes.pop_front();}
  advance(((v ? v : timeout)+999)/1000);
  return v;
}
inline bool ledcWrite(int pin,int duty) {
  if(pin==failingWritePin) return false;
  duties[pin]=duty; return true;
}
inline bool ledcDetach(int pin) {duties[pin]=0; return true;}
inline bool ledcAttachChannel(int,int,int,int) {return true;}
