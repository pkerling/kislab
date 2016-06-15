
#include <Servo.h>

#include "config.h"
#include "utility.h"

#include "speedmonitor.h"
#include "servocontrol.h"
#include "inhibitor.h"
#include "statemachine.h"

Inhibitor inhibitor;
SpeedMonitor hallSpeedMonitor(PIN_HALL_SENSOR);
SpeedMonitor lightSpeedMonitor(PIN_LIGHT_SENSOR, 12, true);
ServoControl servoControl(PIN_SERVO);
StateMachine stateMachine(servoControl, PIN_TRIGGER);
  
void setup() {
  hallSpeedMonitor.setup();
  lightSpeedMonitor.setup();
  servoControl.setup();
  stateMachine.setup();

  hallSpeedMonitor.setCallback([](unsigned long turnTime) {
    inhibitor.hallSpeedCallback(turnTime);
  });
  lightSpeedMonitor.setCallback([](unsigned long turnTime) {
    inhibitor.lightSpeedCallback(turnTime);
  });
  
  stateMachine.setTurnTimeProvider([]() {
    return hallSpeedMonitor.turnTimeUS();
  });
  stateMachine.setLastCrossingTimeProvider([]() {
    return hallSpeedMonitor.lastMeasurementTimeUS();
  });
  
  Serial.begin(250000);
}

void loop() {
  hallSpeedMonitor.loop();
  stateMachine.advanceState();
}
