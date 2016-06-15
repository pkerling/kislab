
#include <Servo.h>

#include "config.h"
#include "utility.h"

#include "speedmonitor.h"
#include "servocontrol.h"
#include "inhibitor.h"
#include "timecalculation.h"
#include "statemachine.h"

Inhibitor inhibitor;
SpeedMonitor hallSpeedMonitor(PIN_HALL_SENSOR);
SpeedMonitor lightSpeedMonitor(PIN_LIGHT_SENSOR, 12, true);
ServoControl servoControl(PIN_SERVO);
TimeCalculation timeCalculation;
StateMachine stateMachine(servoControl);
  
void setup() {
  hallSpeedMonitor.setup();
  lightSpeedMonitor.setup();
  servoControl.setup();
  pinMode(PIN_TRIGGER, INPUT);

  hallSpeedMonitor.setCallback([](unsigned long turnTime) {
    inhibitor.hallSpeedCallback(turnTime);
  });
  lightSpeedMonitor.setCallback([](unsigned long turnTime) {
    inhibitor.lightSpeedCallback(turnTime);
  });

  stateMachine.setTriggerProvider([]() -> bool {
    return digitalRead(PIN_TRIGGER);
  });
  stateMachine.setInhibitionProvider([]() {
    return inhibitor.isInhibited();
  });
  stateMachine.setReleaseTimeCalculator([]() {
    auto now = micros();
    auto lastCrossing = hallSpeedMonitor.lastMeasurementTimeUS();
    auto timeInRound = now - lastCrossing;
    return timeCalculation(hallSpeedMonitor.turnTimeUS(), timeInRound);
  });
  
  Serial.begin(250000);
}

void loop() {
  hallSpeedMonitor.loop();
  lightSpeedMonitor.loop();
  
  stateMachine.advanceState();
}
