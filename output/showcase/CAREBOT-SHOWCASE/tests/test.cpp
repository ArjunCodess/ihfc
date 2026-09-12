#include "Arduino.h"
#include <cassert>
#include <iostream>
uint32_t tick=0;
std::deque<uint32_t> echoes;
std::map<int,int> duties;
std::map<int,int> levels;
bool lineMode=false;
uint32_t movingMs=0;
int failingWritePin=-1;
std::function<int(int)> readHook;
std::function<void(std::string)> logHook;
std::function<void()> timeHook;
SerialMock Serial;
#include "../CareBotESP32/CareBotESP32.ino"
void resetTest() {
  tick=0; lastPingMs=0; aborted=false; attempted=false;
  for(auto &attached:motorAttached) attached=true;
  startArmed=false; startHeld=false; startPressMs=0;
  movingMs=0; failingWritePin=-1; readHook=nullptr; logHook=nullptr; timeHook=nullptr;
  echoes.clear(); duties.clear(); levels.clear(); Serial.input.clear();
  lineMode=false;
  for(auto &r:released) r=false;
}
uint32_t echo(float mm) {return uint32_t(mm*2/0.343f);}
void stopped() {
  for(auto p:MOTOR_PINS) assert(duties[p]==0);
}
int main() {
  resetTest();
  drive(145,145);
  assert(duties[25]==145 && duties[26]==0);
  assert(duties[27]==0 && duties[14]==145); // right motor inverted
  drive(-100,-100);
  assert(duties[25]==0 && duties[26]==100);
  assert(duties[27]==100 && duties[14]==0);
  stopMotors(); stopped();
  resetTest();
  echoes={echo(800),echo(600),echo(220),echo(220),echo(220)};
  assert(wallDistance(220)); stopped();
  resetTest();
  echoes={echo(220),echo(250),echo(400),echo(400),echo(400)};
  assert(wallDistance(400)); stopped();
  resetTest(); echoes={0};
  assert(!wallDistance(220)); assert(aborted); stopped();
  resetTest(); echoes={echo(800),0,0,0};
  assert(!wallDistance(220)); stopped();
  resetTest(); echoes={echo(800),echo(50)};
  assert(!wallDistance(220)); stopped();
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
    uint32_t pulse=SERVO_MIN_US+(SERVO_MAX_US-SERVO_MIN_US)*OPEN_DEG[i]/180;
    assert(duties[SERVO_PINS[i]]==int(pulse*65535UL/20000UL));
  }
  assert(!releaseLoad(0)); stopped();
  resetTest();
  assert(!wallDistance(220)); // fixed far reading must time out
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
  resetTest(); assert(!wallDistance(NAN)); stopped();
  resetTest(); assert(!wallDistance(50)); stopped();
  resetTest(); echoes={echo(800),echo(170),echo(170),echo(170)};
  assert(!wallDistance(220)); stopped(); // overshoot must not authorize a drop
  resetTest(); echoes={echo(220),echo(500),echo(500),echo(500)};
  assert(!wallDistance(400)); stopped();
  resetTest(); echoes={0}; assert(!middleMarker()); assert(movingMs==0);
  resetTest(); failingWritePin=25; drive(100,100);
  assert(aborted && !motorAttached[0]); stopped();
  resetTest(); assert(!servoAngle(5,90)); stopped();
  resetTest(); assert(!servoAngle(0,181)); stopped();
  resetTest(); aborted=true; assert(!servoAngle(0,90));
  resetTest();
  readHook=[](int pin){return pin==START_PIN ? LOW : HIGH;};
  for(int i=0;i<20;++i) loop(); // held during boot cannot start
  assert(!attempted && movingMs==0);

  resetTest();
  readHook=[](int pin) {
    return pin==IR_PIN && (tick<140 || tick>=300) ? LOW : HIGH;
  };
  assert(middleMarker()); assert(tick>=320); stopped();
  resetTest();
  readHook=[](int pin) {
    return pin==IR_PIN && ((tick>=120 && tick<130) || tick>=300) ? LOW : HIGH;
  };
  assert(middleMarker()); assert(tick>=320); stopped(); // ignore short black noise
  resetTest();
  readHook=[](int pin){return pin==IR_PIN ? LOW : HIGH;};
  assert(!middleMarker()); assert(tick>=LEG_TIMEOUT_MS); stopped();
  resetTest();
  timeHook=[](){if(tick>=100 && Serial.input.empty()) Serial.input.push_back('x');};
  assert(!turn90(true)); assert(movingMs<=102); stopped();
  resetTest();
  timeHook=[](){if(tick>=150 && Serial.input.empty()) Serial.input.push_back('x');};
  assert(!moveMm(180)); assert(movingMs<200); stopped();
  resetTest();
  readHook=[](int){return HIGH;}; loop();
  readHook=[](int pin){return pin==START_PIN ? LOW : HIGH;};
  for(int i=0;i<4;++i) loop(); // short bounce
  readHook=[](int){return HIGH;}; loop();
  assert(!attempted && movingMs==0);

  // Integrated route: wall approach, two left turns, line marker, 2/6/2 releases.
  resetTest();
  int phase=0; uint32_t markerStart=0;
  logHook=[&](std::string s){
    if(s.find("Top right ->") == 0) {
      phase=1; echoes={echo(800),echo(220),echo(220),echo(220)};
    }
    if(s.find("Left middle:") == 0) {phase=2; markerStart=tick;}
    if(s.find("Bottom left wall:") == 0) {
      phase=3; echoes={echo(800),echo(220),echo(220),echo(220)};
    }
    if(s.find("Turn left to face right;") == 0) {
      phase=4;
      echoes={echo(800),echo(220),echo(220),echo(220)};
    }
  };
  readHook=[&](int pin){
    return pin==IR_PIN && phase==2 && tick-markerStart>=150 ? LOW : HIGH;
  };
  Serial.input.push_back('s'); loop();
  assert(attempted);
  assert(released[0] && released[1] && released[2]);
  assert(released[3] && released[4] && !aborted);
  stopped();
  uint32_t previousMotion=movingMs;
  Serial.input.push_back('s'); loop(); assert(movingMs==previousMotion);
  resetTest();
  assert(releaseBeams()); assert(movingMs==0);
  assert(released[3] && released[4]); stopped();
  assert(!releaseBeams()); stopped();
  std::cout<<"PASS: full route, open-only servos, stationary beam release, motor/sensor/stop regressions\n";
}

