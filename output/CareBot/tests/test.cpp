#include "Arduino.h"
#include "Wire.h"
#include <cassert>
#include <iostream>
uint32_t tick=0;
std::deque<uint32_t> echoes;
std::map<int,int> duties;
std::map<int,int> levels;
bool lineMode=false;
uint32_t movingMs=0;
int failingWritePin=-1;
bool wireFail=false;
bool pcaDevicePresent=true;
int wireSda=-1, wireScl=-1;
std::map<int,int> servoTicks;
std::function<int(int)> readHook;
std::function<void(std::string)> logHook;
std::function<void()> timeHook;
SerialMock Serial;
TwoWire Wire;
#include "../CareBotESP32/CareBotESP32.ino"
void resetTest() {
  tick=0; lastPingMs=0; aborted=false; attempted=false;
  for(auto &attached:motorPwmAttached) attached=true;
  startupCheckPassed=true;
  movingMs=0; failingWritePin=-1; readHook=nullptr; logHook=nullptr; timeHook=nullptr;
  wireFail=false; pcaDevicePresent=true; wireSda=-1; wireScl=-1; servoTicks.clear();
  servoDriverReady=true;
  echoes.clear(); duties.clear(); levels.clear(); Serial.input.clear();
  lineMode=false;
  for(auto &r:released) r=false;
}
uint32_t echo(float mm) {return uint32_t(mm*2/0.343f);}
void stopped() {
  for(auto p:MOTOR_ENABLE_PINS) assert(duties[p]==0);
  for(auto p:MOTOR_DIRECTION_PINS) assert(levels[p]==LOW);
}
int main() {
  resetTest();
  startupCheckPassed=false; servoDriverReady=false; pcaDevicePresent=false;
  setup();
  assert(aborted && !startupCheckPassed && !servoDriverReady);
  stopped();
  resetTest();
  startupCheckPassed=false; echoes={0};
  setup();
  assert(aborted && !startupCheckPassed);
  stopped();
  resetTest();
  drive(145,145);
  assert(duties[33]==145 && levels[25]==HIGH && levels[26]==LOW);
  assert(duties[17]==145 && levels[27]==LOW && levels[14]==HIGH); // inverted
  drive(-100,-100);
  assert(duties[33]==100 && levels[25]==LOW && levels[26]==HIGH);
  assert(duties[17]==100 && levels[27]==HIGH && levels[14]==LOW);
  stopMotors(); stopped();
  resetTest();
  echoes={echo(800),echo(600),echo(200)};
  assert(approachWall()); assert(movingMs>0); stopped();
  resetTest();
  echoes={echo(180)};
  assert(approachWall()); assert(movingMs==0); stopped();
  resetTest(); echoes={0};
  assert(!approachWall()); assert(aborted); stopped();
  resetTest(); echoes={echo(800),0};
  assert(!approachWall()); assert(aborted); stopped();
  resetTest(); echoes={echo(800),echo(50)};
  assert(approachWall()); stopped();
  resetTest(); Serial.input.push_back('x');
  assert(!turn90(true)); stopped();
  resetTest(); Serial.input.push_back('x');
  assert(!waitChecked(100)); stopped();
  resetTest(); lineMode=true;
  assert(middleMarker()); assert(tick>=220); stopped();
  resetTest();
  assert(!middleMarker()); assert(tick>=LEG_TIMEOUT_MS); stopped();
  resetTest();
  for(unsigned i=0;i<5;++i) assert(releaseLoad(i));
  for(bool r:released) assert(r);
  for(unsigned i=0;i<5;++i) {
    uint32_t pulse=SERVO_MIN_US+(SERVO_MAX_US-SERVO_MIN_US)*RELEASE_DEG[i]/180;
    assert(servoTicks[SERVO_CHANNELS[i]]==int(pulse*4096UL/20000UL));
  }
  assert(!releaseLoad(0)); stopped();
  resetTest();
  assert(!approachWall()); // fixed far reading must time out
  assert(tick>=LEG_TIMEOUT_MS); stopped();
  resetTest();
  assert(moveMm(1.8f)); // 10 ms motion, despite 65 ms initial ping spacing
  assert(movingMs>=10 && movingMs<=11); stopped();
  resetTest();
  assert(moveMm(90)); // 500 ms forward, including periodic echoes
  assert(movingMs>=500 && movingMs<=501); stopped();
  resetTest();
  assert(moveMm(-16)); assert(movingMs==100); stopped();
  resetTest(); assert(!moveMm(NAN)); stopped();
  resetTest(); assert(!moveMm(1e30f)); stopped();
  resetTest(); echoes={echo(800),echo(201),echo(199)};
  assert(approachWall()); stopped(); // only the 20 cm threshold stops travel
  resetTest(); echoes={echo(200)};
  assert(!moveMm(30)); assert(movingMs==0); stopped();
  resetTest(); echoes={echo(800),echo(200)};
  assert(!moveMm(90)); assert(aborted); stopped();
  resetTest(); echoes={0}; assert(!middleMarker()); assert(movingMs==0);
  resetTest(); failingWritePin=33; drive(100,100);
  assert(aborted && !motorPwmAttached[0]); stopped();
  resetTest(); assert(!servoAngle(5,90)); stopped();
  resetTest(); assert(!servoAngle(0,181)); stopped();
  resetTest(); wireFail=true; assert(!servoAngle(0,90)); stopped();
  resetTest(); aborted=true; assert(!servoAngle(0,90));
  resetTest();
  readHook=[](int pin) {
    bool ir = pin==IR_PINS[0] || pin==IR_PINS[1];
    return ir && (tick<140 || tick>=300) ? LOW : HIGH;
  };
  assert(middleMarker()); assert(tick>=320); stopped();
  resetTest();
  readHook=[](int pin) {
    bool ir = pin==IR_PINS[0] || pin==IR_PINS[1];
    return ir && ((tick>=120 && tick<130) || tick>=300) ? LOW : HIGH;
  };
  assert(middleMarker()); assert(tick>=320); stopped(); // ignore short black noise
  resetTest();
  readHook=[](int pin){
    return (pin==IR_PINS[0] || pin==IR_PINS[1]) ? LOW : HIGH;
  };
  assert(!middleMarker()); assert(tick>=LEG_TIMEOUT_MS); stopped();
  resetTest();
  readHook=[](int pin){
    if(pin==IR_PINS[0] && tick>=200) return LOW;
    if(pin==IR_PINS[1] && tick>=400) return LOW;
    return HIGH;
  };
  assert(middleMarker()); assert(tick>=420); stopped(); // both sensors required
  resetTest();
  timeHook=[](){if(tick>=100 && Serial.input.empty()) Serial.input.push_back('x');};
  assert(!turn90(true)); assert(movingMs<=102); stopped();
  resetTest();
  timeHook=[](){if(tick>=150 && Serial.input.empty()) Serial.input.push_back('x');};
  assert(!moveMm(180)); assert(movingMs<200); stopped();
  // Integrated route: wall approach, two left turns, line marker, 2/6/2 releases.
  resetTest();
  int phase=0; uint32_t markerStart=0;
  logHook=[&](std::string s){
    if(s.find("Move to 20 cm wall stop") == 0) {
      phase=1; echoes={echo(800),echo(200)};
    }
    if(s.find("Seek black tape") == 0) {phase=2; markerStart=tick;}
    if(s.find("Move to the next 20 cm wall stop") == 0) {
      phase=3; echoes={echo(800),echo(200)};
    }
    if(s.find("Turn left and move to the next 20 cm wall stop") == 0) {
      phase=4;
      echoes={echo(800),echo(200)};
    }
  };
  readHook=[&](int pin){
    bool ir = pin==IR_PINS[0] || pin==IR_PINS[1];
    return ir && phase==2 && tick-markerStart>=150 ? LOW : HIGH;
  };
  attempted=true;
  assert(runMission());
  assert(released[0] && released[1] && released[2]);
  assert(released[3] && released[4] && !aborted);
  stopped();
  std::cout<<"PASS: automatic route, directional bin servos, motor/sensor/stop regressions\n";
}

