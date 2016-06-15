
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

/**
 * System setup
 * 
 * Instantiate components and connect them.
 */
void setup() {
  Serial.begin(250000);
  
  hallSpeedMonitor.setup();
  lightSpeedMonitor.setup();
  servoControl.setup();
  pinMode(PIN_TRIGGER, INPUT);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);

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
    auto lastCrossing = hallSpeedMonitor.lastMeasurementTime();
    auto timeInRound = now - lastCrossing;
    return timeCalculation(hallSpeedMonitor.turnTime(), timeInRound);
  });

  Serial.println("Setup done");
}

/**
 * Main loop
 */
void loop() {
  hallSpeedMonitor.loop();
  lightSpeedMonitor.loop();
  
  stateMachine.advanceState();

  // Output inhibition state to LED
  digitalWrite(PIN_LED1, inhibitor.isInhibited());
  // Output release state to LED
  digitalWrite(PIN_LED2, (stateMachine.state() == StateMachine::State::WAIT_RELEASE));
}
